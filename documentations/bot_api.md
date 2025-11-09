# Bot Chess API — Documentation minimale

## Description
Cette API permet la communication entre le moteur principal du jeu d’échecs et un bot IA exécuté dans un processus séparé.  
La communication se fait par **pipes** (entrée/sortie standard).

## Structure
### BotConnector
Contient les informations nécessaires pour interagir avec le processus du bot :
- `FILE *bot_in` : flux d’écriture vers le bot (stdin du bot)
- `FILE *bot_out` : flux de lecture depuis le bot (stdout du bot)
- `pid_t pid` : identifiant du processus du bot

## Fonctions principales

### bot_connect
Lance le processus du bot et initialise la communication via pipes.

### is_bot_connected
Vérifie que la connexion au bot est toujours active.

### bot_set_fen
Envoie une position FEN au bot pour mettre à jour son état interne.

### bot_get_best_move
Demande au bot de calculer et renvoyer son meilleur coup.

### bot_play_move
Envoie un coup joué pour que le bot mette à jour sa position interne.

### bot_disconnect
Ferme proprement la communication et termine le processus du bot.

## Protocole de communication

| Commande envoyée | Réponse attendue | Description |
|------------------|------------------|--------------|
| `fen <FEN>` | *(aucune)* | Met à jour la position interne. |
| `get move` | `<MOVE>` | Retourne le meilleur coup. |
| `move <MOVE>` | *(aucune)* | Joue le coup donné. |
| `quit` | *(aucune)* | Termine le bot proprement. |

## Notes
- Le bot doit désactiver le buffering (`setvbuf(stdout, NULL, _IONBF, 0)`).
- Chaque réponse doit se terminer par un saut de ligne (`\n`) et être flushée (`fflush(stdout)`).
