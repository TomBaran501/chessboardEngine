# ♟️ Chessboard Engine

[![Build & Test](https://github.com/<TON-UTILISATEUR>/<TON-REPO>/actions/workflows/build.yml/badge.svg)](https://github.com/<TON-UTILISATEUR>/<TON-REPO>/actions/workflows/build.yml)

Un moteur d’échecs écrit en **C**, utilisant des **bitboards** et testé avec **Criterion**.  
Il permet de charger des positions FEN, de générer tous les coups légaux, et de réaliser des perft tests pour valider la génération des coups.  

---

## 📂 Organisation du projet

    ├── assets/ # Ressources (images,...)
    ├── lib/
    │ └── chessboard/ # Code headers principal du moteur
    ├── src/
    │ └── chessboard/ # Code source principal du moteur
    ├── tests/ # Tests (Criterion)
    ├── Makefile # Compilation
    └── README.md # Ce fichier


---

## ⚡ Installation & Compilation

### Prérequis
- **gcc** ou **clang**
- **make**
- **Criterion** pour les tests  
  ```bash
  sudo apt-get install libcriterion-dev

### Compilation avec fenêtre graphique
  ```bash
  make ui
  ```

### Lancer le moteur
    ```bash
    make run
    ```
### Lancer les tests
   ```bash
  make test
```
### Fonctionnalités

 - Chargement de positions via FEN ✅

 - Génération de tous les coups légaux ✅

 - Vérification via perft test ✅

 - Évaluation de position

 - Implémentation d’un moteur de recherche (minimax / alpha-beta)

### 🧪 CI/CD

Ce projet est testé automatiquement avec GitHub Actions.
Les tests unitaires s’exécutent à chaque push et pull request.