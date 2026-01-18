# PROJECT - Polymorphism : Logiciel de Dessin Vectoriel

Ce projet vise à compléter le développement d'un **logiciel de dessin vectoriel** interactif. Il permet aux utilisateurs de créer et de manipuler des formes géométriques simples (rectangles, cercles, segments, polygones) via une interface graphique simple et un ensemble d’outils spécialisés.

Le projet met en œuvre des concepts clés de la **Programmation Orientée Objet (POO)**, notamment le **polymorphisme** pour l'affichage des formes (`ObjGeom` et ses dérivés) et le traitement des interactions utilisateur (`Tool` et ses dérivés), ainsi que le patron d'architecture **Model-View-Update (MVU)** pour une gestion claire et découplée des données, de l'affichage, et de la logique applicative.

Ce dépôt réalisés dans le cadre du cours de **Programmation C++** dispensé par M. Buzer.

## Source des Exercices et du Cours

Les ressources et les énoncés des travaux proviennent du site de cours officiel :
**[Cours C++ ESIEE Paris - M. Buzer](https://perso.esiee.fr/~buzerl/sphinx_CPPsmart/70_projet/projet.html)**

---

## Objectifs Pédagogiques

Ce projet constitue une démonstration de la compréhension et de la maîtrise de plusieurs concepts fondamentaux en C++, appliqués à la réalisation d’un logiciel de dessin vectoriel interactif.
Il met en avant les compétences suivantes :

* **Programmation orientée objet (POO)**
  Conception de hiérarchies de classes, utilisation du polymorphisme, encapsulation et architecture claire.

* **Gestion des états et programmation événementielle**
  Traitement des interactions clavier/souris, propagation d’évènements, machine à états pour les outils.

* **Structures de données et algorithmes**
  Manipulation de listes d’objets, gestion des hitbox, organisation de scènes, historique Undo.

* **Gestion de la mémoire et pointeurs intelligents**
  Utilisation sûre d’objets dynamiques, stockage polymorphe via `std::unique_ptr` ou équivalent.

* **Architecture logicielle**
  Séparation claire entre données, affichage et logique métier.

* **Sérialisation et persistance**
  Sauvegarde/chargement de la scène, reconstruction des objets à partir d’une représentation textuelle.

* **Gestion graphique et manipulation de figures géométriques**
  Dessin, mise à jour de la forme et des couleurs en temps réel, superposition d’objets.

---

**Auteur :** Xingtong LIN
**Institution :** ESIEE Paris
