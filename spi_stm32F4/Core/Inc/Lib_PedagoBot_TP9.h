#ifndef LIB_PEDAGOBOT_TP9__H
# define LIB_PEDAGOBOT_TP9__H

# include <stm32f4xx_hal.h>
# include <stdio.h>

# define MOTORD 1
# define MOTORG 2
# define MOTOR3 3
// Gains pour le correcteur PI pour l'asservissement en vitesse de chaque moteur/
# define KP 10.0
# define KI 1.0


extern char PRINTF_UART;

void init_PedagoBot_TP9(void);	// Initialisation de l'ensemble des périphériques pour le TP8

// Positionnement des consignes de vitesse pour faire avancer (ou reculer) le moteur motorNum avec une vitesse définie par la variable speed
// La variable speed doit être dans l'intervalle [-100;100] (marche arrière pour une consigne négative)
void setMotorSpeed(unsigned char motorNum, short speed);

void turnAround(void);				// Demi-tour à gauche puis arrèt du robot
void quarterTurnLeft(void);		// Quart de tour à gauche puis arrèt du robot
void quarterTurnRight(void);	// Quart de tour à droite puis arrèt du robot

void initFlag(void);					// Initialisations pour les actions liées au drapeau
void raiseFlag(void);					// Lever le drapeau
void lowerFlag(void);					// Abaisser le drapeau

// Récupération des valeurs des capteurs de réflectance (détection de ligne)
// Pour une ligne noire sur fond clair, choisir Coul_Ligne = 0
// Pour une ligne claire sur fond noir, choisir Coul_Ligne = 1
// Dans tous les cas la variable de retour contient des 1 en cas de détection de la ligne au niveau des bits n°8, 4 et 0 pour les capteurs Gauche, milieu et droit respectivement, 0 partout ailleurs.
unsigned short getLineSensorsValues(unsigned char Coul_Ligne);
	
void SystemClock_Config(void);


#endif
