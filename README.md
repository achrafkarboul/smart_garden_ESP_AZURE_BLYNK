# Smart Garden System

Ce dépôt contient le code source d'un système de jardinage intelligent qui surveille et gère les conditions environnementales de votre jardin. Le système est conçu pour fonctionner avec différents capteurs et dispositifs, tels que le capteur de température et d'humidité DHT11, le capteur d'humidité du sol, et le contrôle d'éclairage.

## Bibliothèques requises

Assurez-vous d'avoir installé les bibliothèques suivantes avant d'exécuter ce programme :

1. WiFi
2. MySQL
3. DHT
4. BlynkSimpleEsp8266

## Variables déclarées

Ce programme utilise diverses variables pour la configuration et la gestion des données :

### Capteurs et Actionneurs

- Capteur de température et d'humidité (DHT)
- Capteur d'humidité du sol
- Capteur de lumière ambiante
- Électrovanne de contrôle d'arrosage

### Configuration de la connexion

- Identifiants WiFi (SSID et mot de passe)
- Clé d'authentification Blynk
- Identifiants de connexion Azure IoT

## Configuration initiale

Le processus de configuration initial est effectué dans la fonction `setup()` :

1. Initialisation des capteurs (DHT et sol)
2. Activation de la connexion WiFi et Blynk
3. Connexion au service Azure IoT

## Boucle principale

La boucle principale du programme (`loop()`) effectue les tâches suivantes :

1. Récupération des données des capteurs (température, humidité, lumière et humidité du sol)
2. Calcul de métriques basées sur les données (indice de stress, point de rosée, etc.)
3. Mise à jour de la base de données Azure avec les données collectées

## Gestion de l'arrosage

Le système de jardinage intelligent prend des mesures d'arrosage en fonction des conditions calculées. L'électrovanne est activée ou désactivée en fonction de l'indice de stress calculé. Si l'indice de stress dépasse un certain seuil, l'électrovanne est activée pour arroser le jardin.

## Intégration avec Blynk

Les données collectées sont également envoyées à la plateforme Blynk pour un suivi en temps réel. Les valeurs d'humidité, de température, d'indice de stress, de point de rosée, de lumière ambiante et d'humidité du sol sont affichées dans l'application Blynk.

## Remarque

Assurez-vous de personnaliser les valeurs de configuration, telles que les identifiants WiFi, la clé d'authentification Blynk et les identifiants Azure IoT, en fonction de votre environnement et de vos besoins spécifiques.

N'oubliez pas de consulter la documentation des différentes bibliothèques utilisées pour une configuration et une utilisation appropriées.