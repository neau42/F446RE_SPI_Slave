# include "Lib_PedagoBot_TP9.h"



// Variables globales
UART_HandleTypeDef huart2;
char PRINTF_UART = 0;
short consigneVitesseMoteurBO1 = 0, consigneVitesseMoteurBO2 = 0; // Valeur de consigne de vitesse asservie Valeur max 600 environ asservissement instable en dessous de 25 environ.
short consigneVitesseMoteurBO1_AV = 0, consigneVitesseMoteurBO2_AV = 0; // pour mémorisation avant arret robot
short mesureVitesseMoteur1 = 0, mesureVitesseMoteur2 = 0;
long int erreurIntegrale1 = 0, erreurIntegrale2 = 0;

// Prototypes
void setMotorSpeedBO(unsigned char motorNum, short speed);

void MX_USART2_UART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // Peripheral clock enable 
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    //USART2 GPIO Configuration    
    //PA2     ------> USART2_TX
    //PA3     ------> USART2_RX 
	
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
    //Error_Handler();
    }
}

 void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    //Configure the main internal regulator output voltage

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    // Initializes the CPU, AHB and APB busses clocks

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 180;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
    //Error_Handler();
    }
    // Activate the Over-Drive mode

    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
    //Error_Handler();
    }
    // Initializes the CPU, AHB and APB busses clocks

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
    //Error_Handler();
    }
}

int fputc(int ch, FILE *f)
{
    if(PRINTF_UART)
    {
        HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
        return ch;
    }
    else
        return ITM_SendChar(ch);
}

// Routine d'interruption périodique (5ms) pour gérer l'asservissement en vitesse des 3 moteurs
void TIM1_BRK_TIM9_IRQHandler(void)//IT_asservissement(void)
{
    //static long int erreurIntegrale1 = 0,erreurIntegrale2 = 0,erreurIntegrale3=0;
    long int erreur1, erreur2;
    int mesV1, mesV2;
    short commandeVitesseMoteur = 0;
    //float KItst;
    double tmp;
    
    if((TIM9->SR & 1) != 0)                        // Si le drapeau d'interruption du Timer9 est levé
    {
        // ********************** //
        // Asservissement moteur1 //
        // ********************** //
        if (mesureVitesseMoteur1 != 0)
            mesV1 = 100000 / mesureVitesseMoteur1;                                    // Récupération de la mesure
        else
            mesV1 = 0;
        if((consigneVitesseMoteurBO1 > -25) && (consigneVitesseMoteurBO1 < 25))      // Saturation basse à 0 pour éviter l'instabilité dans les faibles vitesses
            setMotorSpeedBO(MOTORD, 0);                                              // On impose une vitesse nulle dans ce cas
        else
        {      
            erreur1 = consigneVitesseMoteurBO1 - mesV1;                             // Calcul de l'erreur à l'instant t
            if((erreurIntegrale1+erreur1) < 20000 && (erreurIntegrale1+erreur1) > -20000)
                erreurIntegrale1 += erreur1;                                        // Mise à jour de l'erreur intégrale avec saturation
            //KItst = 0.0024*abs(consigneVitesseMoteurBO1)+0.18;                    // Test d'un coefficient Ki variable pour que le temps de réponse soit le même pour les faibles consignes et les plus élevées
            tmp = (KP * erreur1 + KI * erreurIntegrale1);
            if (tmp > 4500.0)                                                         // Saturation ici pour éviter le débordement dans le passa de l'argument à la fonction setMotorSpeedBO
                commandeVitesseMoteur = 4500;
            else if (tmp < -4500.0)
                commandeVitesseMoteur = -4500;
            else
                commandeVitesseMoteur = (short)tmp;
            setMotorSpeedBO(MOTORD, commandeVitesseMoteur);                         // Envoi de la commande de vitesse
         }

        // ********************** //
        // Asservissement moteur2 //
        // ********************** //
        if (mesureVitesseMoteur2 != 0)
            mesV2 = 100000 / mesureVitesseMoteur2;                                    // Récupération de la mesure
        else
            mesV2 = 0;
        if((consigneVitesseMoteurBO2 > -25) && (consigneVitesseMoteurBO2 < 25))      // Saturation basse à 0 pour éviter l'instabilité dans les faibles vitesses
            setMotorSpeedBO(MOTORG, 0);                                              // On impose une vitesse nulle dans ce cas
        else
        {      
            erreur2 = consigneVitesseMoteurBO2 - mesV2;                             // Calcul de l'erreur à l'instant t
            if((erreurIntegrale2 + erreur2) < 20000 && (erreurIntegrale2 + erreur2) > -20000)
                erreurIntegrale2 += erreur2;                                        // Mise à jour de l'erreur intégrale avec saturation
            //KItst = 0.0024*abs(consigneVitesseMoteurBO2)+0.18;                    // Test d'un coefficient Ki variable pour que le temps de réponse soit le même pour les faibles consignes et les plus élevées
            tmp = (KP * erreur2 + KI * erreurIntegrale2);
            if (tmp > 4500.0)                                                         // Saturation ici pour éviter le débordement dans le passa de l'argument à la fonction setMotorSpeedBO
                commandeVitesseMoteur = 4500;
            else if (tmp < -4500.0)
                commandeVitesseMoteur = -4500;
            else
                commandeVitesseMoteur = (short)tmp;
            setMotorSpeedBO(MOTORG, commandeVitesseMoteur);                         // Envoi de la commande de vitesse
         }  
        TIM9->SR &= ~1;                   // Mise à zÈro du drapeau d'interruption    
    }
}

// Routine d'interuption du Timer 3 permettant de gérer la mesure de vitesse pour le moteur 2 et la mise à jour de l'odométrie
void TIM3_IRQHandler() //IT_codeurTim3(void){
{
    static int mauvaiseMesure2 = 0;              // Mémorisation de l'info de mauvaise mesure (débordement du timer)
    if ((TIM3->SR & 1) != 0)                     // Déclenchement de l'IT à cause de "Update event" (le CPT à débordé -> mauvaise mesure)
    {
        mauvaiseMesure2 = 1;                     // On mémorise que la prochaine mesure doit être jetée
        mesureVitesseMoteur2 = 0;                // Au dela d'une période de 10ms, on considère que la vitesse est nulle
        TIM3->SR &= ~1;                          // Mise à zéro du drapeau d'interruption (Validation de l'IT)
    }
    else if ((TIM3->SR & (1 << 1)) != 0)         // Déclenchement de l'IT à cause de "Input capture" un front est apparu sur le codeur, une nouvelle valeur de période est dispo
    {
        //if(((GPIOA->IDR&(1 << 7))>>7)!=((GPIOA->IDR&(1<<6)))>>6 ){ // test du sens de rotation : si PA6==PA7 ->sens1, sinon sens2
        if(((GPIOB->IDR & (1 << 4)) >> 4) != ((GPIOB->IDR & (1 << 5))) >> 5) // test du sens de rotation : si PB4==PB5 ->sens1, sinon sens2
        {
            if (mauvaiseMesure2 == 1)            // Si la précédente valeur était mauvaise...
                mauvaiseMesure2 = 0;             // On réinitialise la variable
            else                                 // La mesure est bonne
            mesureVitesseMoteur2 = TIM3->CNT;
            /*// Mise à jour de l'odométrie, que la mesure soit bonn ou non
            X_a -= dx*sin(THETA_a+alpha1);
            Y_a += dx*cos(THETA_a+alpha1);
            THETA_a +=dTheta;*/
        }
        else
        {
            if (mauvaiseMesure2 == 1)            // Si la précédente valeur était mauvaise...
                mauvaiseMesure2 = 0;             // On réinitialise la variable
            else // La mesure est bonne
                mesureVitesseMoteur2 = -TIM3->CNT;
            /*// Mise à jour de l'odométrie, que la mesure soit bonn ou non
            X_a += dx*sin(THETA_a+alpha1);
            Y_a -= dx*cos(THETA_a+alpha1);
            THETA_a -=dTheta;  */ 
        }
//        if( THETA_a>M_PI) THETA_a -= (2*M_PI);   // La valeur de THETA_a doit rester dans ]-Pi;Pi]
//        else if( THETA_a<=-M_PI) THETA_a += (2*M_PI);
        TIM3->CNT = 0;                          // Réinitialisation du compteur pour la prochaine capture
        TIM3->SR &= ~(1 << 1);                    // Mise à zéro du drapeau d'interruption (Validation de l'IT)
    }
}

// Routine d'interuption du Timer 4 permettant de gérer la mesure de vitesse pour le moteur 3 et la mise à jour de l'odométrie
void TIM4_IRQHandler()//IT_codeurTim4(void){
{
    static int mauvaiseMesure1 = 0;              // Mémorisation de l'info de mauvaise mesure (débordement du timer)
    if((TIM4->SR & 1) != 0)                     // Déclenchement de l'IT à cause de "Update event" (le CPT à débordé -> mauvaise mesure)
    {
        mauvaiseMesure1 = 1;                      // On mémorise que la prochaine mesure doit être jetée
        mesureVitesseMoteur1 = 0;               // Au dela d'une période de 10ms, on considère que la vitesse est nulle
        TIM4->SR &= ~1;                          // Mise à zéro du drapeau d'interruption (Validation de l'IT)
    }
    else if ((TIM4->SR & (1 << 1)) != 0)        // Déclenchement de l'IT à cause de "Input capture" un front est apparu sur le codeur, une nouvelle valeur de période est dispo
    {
        if(((GPIOB->IDR & (1 << 7)) >> 7) != ((GPIOB->IDR & (1 << 6))) >> 6) // test du sens de rotation : si PB6==PB7 ->sens1, sinon sens2
        {
            if (mauvaiseMesure1 == 1)             // Si la précédente valeur était mauvaise...   
                mauvaiseMesure1 = 0;              // On réinitialise la variable
             else
                mesureVitesseMoteur1 = TIM4->CNT;// La mesure est bonne
        /*// Mise à jour de l'odométrie, que la mesure soit bonn ou non
            X_a -= dx*sin(THETA_a+alpha2);
            Y_a += dx*cos(THETA_a+alpha2);
            THETA_a +=dTheta; */
        }
        else
        {
            if (mauvaiseMesure1 == 1)             // Si la précédente valeur était mauvaise...   
                mauvaiseMesure1 = 0;              // On réinitialise la variable
             else                                 // La mesure est bonne
                mesureVitesseMoteur1 = -TIM4->CNT;
        /*// Mise à jour de l'odométrie, que la mesure soit bonn ou non
            X_a += dx*sin(THETA_a+alpha2);
            Y_a -= dx*cos(THETA_a+alpha2);
            THETA_a -=dTheta; */ 
        }
//        if( THETA_a>M_PI) THETA_a -= (2*M_PI);   // La valeur de THETA_a doit rester dans ]-Pi;Pi]
//        else if( THETA_a<=-M_PI) THETA_a += (2*M_PI);
        TIM4->CNT = 0;                          // Réinitialisation du compteur pour la prochaine capture
        TIM4->SR &= ~(1 << 1);                    // Mise à zéro du drapeau d'interruption (Validation de l'IT)
    }
}

void motorsEnable(void)
{
    TIM1->CCMR1 |= ((3 << 5) | (3 << 13));
    TIM1->CCMR1 &= ~((1 << 4) | (1 << 12));
    TIM1->CCMR2 |= ((3 << 5) | (3 << 13));
    TIM1->CCMR2 &= ~((1 << 4) | (1 << 12)); 
    erreurIntegrale1 = 0;
    erreurIntegrale2 = 0;
    TIM1->BDTR |= ((1 << 15));// Break : désactivation de toutes les sorties OCx 
}

void motorsBreak(void)
{
    TIM1->CCMR1 |= ((1 << 6) | (1 << 14));
    TIM1->CCMR1 &= ~((3 << 4) | (3 << 12));
    TIM1->CCMR2 |= ((1 << 6) | (1 << 14));
    TIM1->CCMR2 &= ~((3 << 4) | (3 << 12));
    TIM1->BDTR &= ~((1 << 15));// Break disable : activation de toutes les sorties OCx   
}

// Impose une valeur de vitesse en boucle ouverte pour un moteur donné (directement dans le registre de comparaison) Speed dans [-4500;4500]
void setMotorSpeedBO(unsigned char motorNum, short speed)
{
    unsigned short absSpeed, sens=0;
    // Récupération de la valeur absolue et du sens
    if (speed<0)
    {
        absSpeed=-speed;
        sens = 0;
    }
    else
    {
        absSpeed = speed;
        sens = 1;
    }
    if (absSpeed > 4500)     // Saturation au max en cas de commande trop élevée
        absSpeed = 4500;
    switch(motorNum)
    {
        case MOTORD :
            TIM1->CCR3 = absSpeed;                 // rapport cyclique du PWM pour le moteur 1 // PWM sur PA10
            GPIOA->BSRR = (1 << (7 + 16 * !sens)); // Mise à "sens" du signal de direction //DIR sur PA7
            break;
        case MOTORG :
            TIM1->CCR1 = absSpeed;                 // rapport cyclique du PWM pour le moteur 2 // PWM sur PA8
            GPIOA->BSRR = (1 << (11 + 16 * sens)); // Mise à "sens" du signal de direction //DIR sur PA11
            break;
        case MOTOR3 :
            TIM1->CCR2 = absSpeed;                  // rapport cyclique du PWM pour le moteur 3 // PWM sur PA9
            GPIOA->BSRR = (1 << (12 + 16 * !sens)); // Mise à "sens" du signal de direction //DIR sur PA12
            break;
/*        case MOTOR4 : 
            TIM1->CCR4 = absSpeed;                  // rapport cyclique du PWM pour l'éventuel moteur 4
            GPIOA->BSRR = (1<<(3+16*sens));         // Mise à "sens" du signal de direction
            break;
*/
    }
} 
// Impose la vitesse de consigne du moteur n°motorNum en Boucle fermée (asservi)
// La vitesse de consigne doit être inférieure à Vmax (environ 900) en valeur absolue.
void setMotorSpeedBF(unsigned char motorNum, short speed)
{
    switch(motorNum)
    {
        case MOTORD :
            consigneVitesseMoteurBO1 = speed;
            break;
        case MOTORG : 
            consigneVitesseMoteurBO2 = speed;
            break;    
        case MOTOR3 : 
            break;
    }
}

void start_asserv(void)
{
    TIM9->CR1 |= 1;                  // Activation du Timer9 (demmarage des interruptions périodiques pour l'asservissement)
}

void stop_asserv(void)
{
    TIM9->CR1 &= ~1;                  // Desactivation du Timer9 (arret des interruptions périodiques pour l'asservissement)
    setMotorSpeedBO(MOTORD, 0);       // Roue droite
    setMotorSpeedBO(MOTORG, 0);       // Roue gauche
}


// Timer 1 pour générer les 3 PWM (2 commande des moteurs de déplacement et 1 libre)
void init_TIM1_PWMx4(void)
{
    ///// Horloge /////
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; //GPIOAEN='1' -> activation de l'horloge sur le port A
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;  // Activation Horloge sur TIM1 (180MHz)

    ///// GPIO /////
    // PA8, PA9 et PA10  en alternate fonction
    GPIOA->MODER |= ((1 << 17) | (1 << 19) | (1 << 21));
    GPIOA->MODER &= ~((1 << 16) | (1 << 18) | (1 << 20));
    //GPIOA->OSPEEDR |= 0x30000; // Set the PWM outputs to high speed (OC1)
    
    // AF1 pour PA8, PA9 et PA10 (pour TIM1_CH1, TIM1_CH2 et TIM1_CH3)
    // AFRH8[3:0]=0001, AFRH9[3:0]=0001, AFRH10[3:0]=0001
    GPIOA->AFR[1] |= ((1 << 0) | (1 << 4) | (1 << 8));
    GPIOA->AFR[1] &= ~((7 << 1) | (7 << 5) | (7 << 9));
    

    ///// TIM1 /////
    TIM1->PSC = 0;      // pas de prédivision, le compteur compte à 84Mhz
    TIM1->ARR = 4500;   // Valeur pour une période de PWM de 20KHz en mode center-aligned
    TIM1->EGR |= 1;      // Mise à 1 bit UG de EGR pour provoqer chargement de PSC
    
    TIM1->CR1 |= ((1 << 7) | (1 << 5));   // Mode de comptage center-aligned, mode auto-reload preload activé   
    TIM1->CR2 = 0;
    TIM1->SMCR = 0;
    TIM1->DIER = 0;
  
    //OC1M[2:0] = "110" et OC2M[2:0] = "110" PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 else inactive
    TIM1->CCMR1 |= ((3 << 5) | (3 << 13));
    TIM1->CCMR1 &= ~((1 << 4) | (1 << 12));
    // CC1S[1:0]=CC2S[1:0]="00" : CC1 and CC2 channel is configured as output
    TIM1->CCMR1 &= ~((3 << 0) | (3 << 8));
    
    //OC3M[2:0] =OC4M[2:0] = "110" : PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 else inactive
    TIM1->CCMR2 |= ((3 << 5) | (3 << 13));
    TIM1->CCMR2 &= ~((1 << 4) | (1 << 12));
    // CC3S[1:0]=CC4S[1:0]="00" : CC3 channel is configured as output
    TIM1->CCMR2 &= ~((3 << 0) | (3 << 8));
    
    TIM1->CCER |= ((1 << 0) | (1 << 4) | (1 << 8) | (1 << 12)); // sorties OC1, OC2, OC3 et OC4 actives
    
    // Initialisation des rapports cycliques 
    TIM1->CCR1 = 0;
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;
    TIM1->CCR4 = 0;
    
    //Activation des sorties OCx
    TIM1->BDTR |= ((1 << 15)); // MOE=1 pour activer les sorties OC
  
    TIM1->CR1|=1;       // Enable TIM1
}

// Timer 8 pour générer les 4 PWM pour les commandes de servomoteurs (sur PC6, PC7, PC8 et PC9)
void initFlag(void){
    ///// Horloge /////
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;  //GPIOCEN='1' -> activation de l'horloge sur le port C
    RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;  // Activation Horloge sur TIM8 (180MHz)

    ///// GPIO /////
    // PC6, PC7, PC8 et PC9  en alternate fonction
    GPIOC->MODER |= ((2 << 12) | (2 << 14) | (2 << 16) | (2 << 18));
    GPIOC->MODER &= ~((1 << 12) | (1 << 14) | (1 << 16) | (1 << 18));   
    
    // AF3 pour PC6, PC7, PC8 et PC9 (pour TIM8_CH1, TIM8_CH2, TIM8_CH3 et TIM8_CH4)
    // AFRH6[3:0]=0011, AFRH7[3:0]=0011, AFRH8[3:0]=0011, AFRH9[3:0]=0011
    GPIOC->AFR[0] &= ~((15 << 24) | (15 << 28));
    GPIOC->AFR[0] |= ((3 << 24) | (3 << 28));
    GPIOC->AFR[1] &= ~((15 << 0) | (15 << 4));
    GPIOC->AFR[1] |= ((3 << 0) | (3 << 4));

    ///// TIM8 /////
    TIM8->PSC = 179;     // Prédivision par 180, le compteur s'incrémente chaque 1us
    TIM8->ARR = 4999;   // Valeur pour une période de PWM de 10ms en mode center-aligned
    TIM8->EGR |=1;      // Mise à 1 bit UG de EGR pour provoqer chargement de PSC
    
    TIM8->CR1|=((1 << 7) | (1 << 5));   // Mode de comptage center-aligned, mode auto-reload preload activé   
    TIM8->CR2 = 0;
    TIM8->SMCR=0;
    TIM8->DIER=0;
  
    //OC1M[2:0] = "110" et OC2M[2:0] = "110" PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 else inactive
    TIM8->CCMR1 |= ((3 << 5) | (3 << 13));
    TIM8->CCMR1 &= ~((1 << 4) | (1 << 12));
    // CC1S[1:0]=CC2S[1:0]="00" : CC1 and CC2 channel is configured as output
    TIM8->CCMR1 &= ~((3 << 0) | (3 << 8));
    
    //OC3M[2:0] =OC4M[2:0] = "110" : PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 else inactive
    TIM8->CCMR2 |= ((3 << 5) | (3 << 13));
    TIM8->CCMR2 &= ~((1 << 4) | (1 << 12));
    // CC3S[1:0]=CC4S[1:0]="00" : CC3 channel is configured as output
    TIM8->CCMR2 &= ~((3 << 0) | (3 << 8));
    
    TIM8->CCER |= ((1 << 0) | (1 << 4) | (1 << 8) | (1 << 12)); // sorties OC1, OC2, OC3 et OC4 actives
    
    // Initialisation des rapports cycliques 
    TIM8->CCR1 = 0;
    TIM8->CCR2 = 0;
    TIM8->CCR3 = 0;
    TIM8->CCR4 = 0;
    
    //Activation des sorties OCx
    TIM8->BDTR |= ((1 << 15)); // MOE=1 pour activer les sorties OC
  
    TIM8->CR1|=1;       // Enable TIM8
}

void init_ES(void){
    ///// Horloge /////
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN|RCC_AHB1ENR_GPIOCEN|RCC_AHB1ENR_GPIODEN|RCC_AHB1ENR_GPIOBEN;  // GPIOAEN=GPIOBEN=GPIODEN=GPIOCEN='1' -> activation de l'horloge sur les ports A, B et C
    
    // PA7, PA11 et PA12 en sortie (DIR pour chaque pont en H)
    GPIOA->MODER |= ((1 << 14) | (1 << 22) | (1 << 24));
    GPIOA->MODER &= ~((2 << 14) | (2 << 22) | (2 << 24));
    
    // PC13 en entrée -> bouton USER
    GPIOC->MODER &= ~((3 << 26));  
    // PA5 en sortie -> LED Verte
    GPIOA->MODER |= ((1 << 10));
    GPIOA->MODER &= ~((2 << 10));    
		// PD2 en sortie -> LED Capteur Couleur
    GPIOD->MODER |= ((1 << 4));
    GPIOD->MODER &= ~((2 << 4));
    
  // Capteurs de ligne OPB705 -> PB15, PB1 et PB2 en entrée
    GPIOB->MODER &= ~((3 << 30) | (3 << 2) | (3 << 4));
}

void init_encoders_Periode(void){
      ///// Horloge //////
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; //  GPIOBEN = '1'  -> activation de l'horloge sur le port B
    RCC->APB1ENR |= ((1 << 1) | (1 << 2));    // Activation Horloge sur TIM3 et TIM4 (84MHz)
    
    ///// GPIO /////

    //PB4 en AF MODER4[1:0] = "10" et PB5 en entrée MODER5[1:0] = "00"
    GPIOB->MODER |= ((1 << 9));
    GPIOB->MODER &= ~((1 << 8) | (1 << 10) | (1 << 11)); 
    GPIOB->PUPDR |= ((1 << 8) | (1 << 10)); // Pull-up sur PB4 et PB5
    GPIOB->PUPDR &= ~((1 << 9) | (1 << 11));  
    // AF2 pour PB4 (pour TIM3_CH1)
    // AFRH4[3:0]="0010" 
    GPIOB->AFR[0] |= ((1 << 17));
    GPIOB->AFR[0] &= ~((1 << 16) | (3 << 18));
    
    //PB6 en AF MODER6[1:0] = "10" et PB7 en entrée MODER7[1:0] = "00"
    GPIOB->MODER |= ((1 << 13));
    GPIOB->MODER &= ~((1 << 12) | (1 << 14) | (1 << 15)); 
    GPIOB->PUPDR |= ((1 << 12) | (1 << 14)); // Pull-up sur PB6 et PB7
    GPIOB->PUPDR &= ~((1 << 13) | (1 << 15));  
    // AF2 pour PB6 (pour TIM4_CH1)
    // AFRH6[3:0]="0010" 
    GPIOB->AFR[0] |= ((1 << 25));
    GPIOB->AFR[0] &= ~((1 << 24) | (3 << 26));
        
    ///// TIM3 /////
    TIM3->PSC = 179;            // Prédivison par 180 donc T_CK_CNT=1us
    TIM3->ARR = 49999;          // Valeur max correspondant à une période de 50ms (au dela de cela on considèrera une vitesse nulle).
    TIM3->EGR |= 1;              // Mise à 1 bit UG de EGR pour provoqer chargement de PSC
    
    TIM3->CR1 = (1 << 7);     // Mode auto-reload preload activé   
    TIM3->CR2 = 0;
    TIM3->SMCR = 0;
 
    TIM3->CCMR1 = 0x0031;         // Activation d'un filtrage de 8 échantillon sur l'entrée pour éviter le déclenchement sur parasites + Mode Input Capture sur a voie 1
    TIM3->CCMR2 = 0;
    TIM3->CCER = (1 << 1) | (1 << 3); // Déclenchement de la capture sur les deux front, Montant ET descendant de la voie 1
    
    TIM3->SR &= ~3;               // Abaissement des 2 flags pour démarrer les IT proprement
    TIM3->DIER = (1 << 0) | (1 << 1);   // Activation des IT sur input capture et update event (valeur max atteinte).
    // Remapping de la routine d'interruption
    //NVIC_SetVector(TIM3_IRQn,(uint32_t)IT_codeurTim3);
    NVIC_SetPriority(TIM3_IRQn, 0);
    // Activation de l'interruption par le controleur d'IT 
    NVIC_EnableIRQ(TIM3_IRQn);
    
    TIM3->CCER |= (1 << 0);     // Activation de la capture du TIM3 
    TIM3->CR1 |= (1 << 0);      // Activation finale du TIM3   
    
    ///// TIM4 /////
    TIM4->PSC = 179;            // Prédivison par 180 donc T_CK_CNT=1us
    TIM4->ARR = 49999;          // Valeur max correspondant à une période de 50ms (au dela de cela on considèrera une vitesse nulle).
    TIM4->EGR |= 1;             // Mise à 1 bit UG de EGR pour provoqer chargement de PSC
    
    TIM4->CR1 = (1 << 7);       // Mode auto-reload preload activé
    TIM4->CR2 = 0;
    TIM4->SMCR = 0;
 
    TIM4->CCMR1 = 0x0031;             // Activation d'un filtrage de 8 échantillon sur l'entrée pour éviter le déclenchement sur parasites + Mode Input Capture sur a voie 1
    TIM4->CCMR2 = 0;
    TIM4->CCER = (1 << 1) | (1 << 3); // Déclenchement de la capture sur les deux front, Montant ET descendant de la voie 1
    
    TIM4->SR &= ~3;                   // Abaissement des 2 flags pour démarrer les IT proprement
    TIM4->DIER=(1 << 0) | (1 << 1);   // Activation des IT sur input capture et update event (valeur max atteinte).
    // Remapping de la routine d'interruption
    //NVIC_SetVector(TIM4_IRQn,(uint32_t)IT_codeurTim4);
    // Activation de l'interruption par le controleur d'IT 
    NVIC_SetPriority(TIM4_IRQn,0);
    NVIC_EnableIRQ(TIM4_IRQn);
    
    TIM4->CCER |= (1 << 0);     // Activation de la capture du TIM3 
    TIM4->CR1 |= (1 << 0);      // Activation finale du TIM3  
}
// Timer 9 pour la période d'échantillonage de l'asservissement
void init_Timer9_IT_Xus(unsigned int x)
{
    RCC->APB2ENR |= (1 << 16);       // Activation de l'horloge sur le Timer9
    TIM9->PSC = 179;                 // Prescaler de 180, donc T_CK_CNT=180/180M = 1 us
    TIM9->ARR = x-1;                 // Comptage de 0 à x, donc un cycle de comptage en x us
    TIM9->EGR |= 1;                  // Mise à 1 bit UG de EGR pour provoq chargt de PSC
    
    TIM9->SR &= ~1;                  //Mise à zéro du drapeau d'interruption pour démarrer dans de bones conditions
    TIM9->DIER |= 1;                 // Activation de l'interruption sur l'evt de mise à jour (overflow)
    // Remapping de la routine d'interruption
    //NVIC_SetVector(TIM1_BRK_TIM9_IRQn,(uint32_t)IT_asservissement); 
    //Enable TIM1_update IRQ ->équivalent à la ligne suivante
    NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 3);
    NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
   // NVIC->ISER[0]=1 << 25;         // Activation de l'IT du timer9 (position 25 dans le tableau des IT)
    //TIM9->CR1 |=1;                 // Activation du Timer1 (demmarage du comptage)
        
}
// Timer 10 pour la charge du processeur
void init_Timer10_IT_100us(void)
{
    RCC->APB2ENR |= (1 << 17);       // Activation de l'horloge sur le Timer10
    TIM10->PSC = 179;                // Prescaler de 180, donc T_CK_CNT=180/180M = 1 us
    TIM10->ARR = 99;                 // Comptage de 0 à x, donc un cycle de comptage en x us
    TIM10->EGR |= 1;                 // Mise à 1 bit UG de EGR pour provoq chargt de PSC
    
    TIM10->SR &= ~1;                 //Mise à zéro du drapeau d'interruption pour démarrer dans de bones conditions
    TIM10->DIER |= 1;                // Activation de l'interruption sur l'evt de mise à jour (overflow)
    // Remapping de la routine d'interruption
	
    //NVIC_SetVector(TIM1_BRK_TIM9_IRQn,(uint32_t)IT_asservissement); 
    //Enable TIM1_update IRQ ->équivalent à la ligne suivante
    NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 12);// moins prioritaire que les autres IT

    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    
   // NVIC->ISER[0]=1 << 25;        // Activation de l'IT du timer9 (position 25 dans le tableau des IT)
    //TIM10->CR1 |=1;               // Activation du Timer10 (demmarage du comptage)
}

void init_TIM6_base_500us(void)
{
	RCC->APB1ENR |= (1 << 4);
	TIM6->CR1 = (1 << 3);
	TIM6->PSC = 44999;// T_CK_CNT = 500us 
	//TIM6->ARR = 99;
	TIM6->EGR |= 1;
}

void tempo_TIM6_x_500us(unsigned int x)
{
		TIM6->ARR = x;
		TIM6->CR1 |= 1;
		while((TIM6->CR1&1) == 1)
            ;
}
// 
void init_PedagoBot_TP9(void)
{
    HAL_Init();
    SystemClock_Config();
    init_TIM1_PWMx4();
    //initFlag();
    init_ES();
    init_encoders_Periode();
    init_Timer9_IT_Xus(5000); // Période de 5 ms pour l'asservissement
    init_Timer10_IT_100us();
    init_TIM6_base_500us();
    if(PRINTF_UART)
        MX_USART2_UART_Init();
    start_asserv();
    //printf("Initialisation finie !!!!!\n\r");
}

//void SysTick_Handler(void)
//{
//  HAL_IncTick();
//}


// Positionnement des consignes de vitesse pour avancer (ou reculer) tout droit avec une vitesse définie
// La variable speed doit être dans l'intervalle [-100;100] (marche arrière pour une consigne négative)
void setRobotSpeed_Straight(short speed)
{
    short speed_sat;
    short consigne;
    if (speed > 100)
        speed_sat=100;
    else if (speed <- 100)
        speed_sat = -100;
    else
        speed_sat = speed;
    consigne = (short)speed_sat * 4;
    setMotorSpeedBF(MOTORD, consigne);
    setMotorSpeedBF(MOTORG, consigne);
}

void LED_Capteur_Couleur(unsigned char etat)
{
	GPIOD->BSRR = 1 << (2 + 16 * !etat);
}
// Positionnement des consigne de vitesse pour imposer une rotation lente à gauche
void rotateLeft(void)
{
    setMotorSpeedBF(MOTORD, 100);
    setMotorSpeedBF(MOTORG, -100);
}	

// Positionnement des consigne de vitesse pour imposer une rotation lente à droite
void rotateRight(void)
{
	setMotorSpeedBF(MOTORD, -100);
	setMotorSpeedBF(MOTORG, 100);
}

// Positionnement des consigne de vitesse pour avancer tout droit à vitesse lente
void goStraight(void)
{
	setMotorSpeedBF(MOTORD, 100);
	setMotorSpeedBF(MOTORG, 100);
}

// Positionnement des consigne de vitesse pour reculer tout droit à vitesse lente
void goBackwards(void)
{
	setMotorSpeedBF(MOTORD, -100);
	setMotorSpeedBF(MOTORG, -100);
}	

// Positionnement des consignes de vitesse nulles
void stopRobot(void)
{
	setMotorSpeedBF(MOTORD,0);
	setMotorSpeedBF(MOTORG,0);
}

void load_Processor(void)
{
	TIM10->CR1 |= 1;                  // Activation du Timer10 (demmarage du comptage)
}

// Routine d'interruption périodique (100us) pour charger le processeur
void TIM1_UP_TIM10_IRQHandler(void)
{
    int i = 100000000;

    if((TIM10->SR & 1) != 0)       // Si le drapeau d'interruption du Timer10 est levé
    {
			while(i > 0)
				i--;
        TIM10->SR &= ~1;                   // Mise à zÈro du drapeau d'interruption    
    }
}

void trajectory_square_80cm(void)
{
	for (int i = 0; i < 4; i++)
    {
        goStraight();
        tempo_TIM6_x_500us(16000);
        //tempo_logicielle(t1);
        rotateLeft();
        tempo_TIM6_x_500us(3280);
        //tempo_logicielle(t2);
	}
	stopRobot();
}

void suspend_trajectory(void)
{
	consigneVitesseMoteurBO1_AV = consigneVitesseMoteurBO1;
	consigneVitesseMoteurBO2_AV = consigneVitesseMoteurBO2;
	stopRobot();
	TIM6->CR1 &= ~1;// Desactivation du Timer
}

void resume_trajectory(void)
{
	consigneVitesseMoteurBO1 = consigneVitesseMoteurBO1_AV;
	consigneVitesseMoteurBO2 = consigneVitesseMoteurBO2_AV;
	TIM6->CR1 |= 1;// Activation du Timer
}

// Positionnement des consignes de vitesse pour faire avancer (ou reculer) le moteur motorNum avec une vitesse définie par la variable speed
// La variable speed doit être dans l'intervalle [-100;100] (marche arrière pour une consigne négative)
void setMotorSpeed(unsigned char motorNum, short speed)
{
    short speed_sat;
    short consigne;
    if (speed > 100)
        speed_sat = 100;
    else if (speed < -100)
        speed_sat = -100;
    else
        speed_sat = speed;
    consigne = (short)speed_sat * 4;
	setMotorSpeedBF(motorNum, consigne);
}	
void turnAround(void)
{
    rotateLeft();
    tempo_TIM6_x_500us(3280);
    tempo_TIM6_x_500us(3280);
    stopRobot();
    tempo_TIM6_x_500us(1000);
}

void quarterTurnLeft(void)
{
    rotateLeft();
    tempo_TIM6_x_500us(3280);
    stopRobot();
    tempo_TIM6_x_500us(1000);
}

void quarterTurnRight(void)
{
    rotateRight();
    tempo_TIM6_x_500us(3280);
    stopRobot();
    tempo_TIM6_x_500us(1000);
}

void raiseFlag(void)
{
	TIM8->CCR4 = 800 / 2;
}

void lowerFlag(void)
{
	TIM8->CCR4 = 1800 / 2;
}

// Récupération des valeurs des capteurs de réflectance (détection de ligne)
// Pour une ligne noire sur fond clair, choisir Coul_Ligne = 0
// Pour une ligne claire sur fond noir, choisir Coul_Ligne = 1
// Dans tous les cas la variable de retour contient des 1 en cas de détection de la ligne au niveau des bits n°8, 4 et 0 pour les capteurs Gauche, milieu et droit respectivement, 0 partout ailleurs.
unsigned short getLineSensorsValues(unsigned char Coul_Ligne)
{
    unsigned short res = 0, mskk=0;
    // mskk = 0xFFFF pour ligne réfléchissante sur fond sombre
    // mskk = 0 pour ligne sombre sur fond réfléchissant
    if (Coul_Ligne == 1)
        mskk = 0xFFFF;
    else
       mskk = 0;
	res |= ((GPIOB->IDR ^ mskk) & (1 << 2)) << 6;   // valeur capteur Gauche décalé au bit8
	res |= ((GPIOB->IDR ^ mskk) & (1 << 1)) << 3;	// valeur capteur Milieu décalé au bit4
	res |= ((GPIOB->IDR ^ mskk) & (1 << 15)) >> 15; // valeur capteur Droite décalé au bit0
	return res;
}
