#pragma once
#include "quadtree.h"
#include <algorithm>
#include <Imagine/Graphics.h>
using namespace Imagine;
using namespace std;

const int SEUIL = 10;

QuadTree<byte> *NOIR = new QuadLeaf<byte>(byte(0));    // Variable globale : pointeur feuille noire
QuadTree<byte> *BLANC = new QuadLeaf<byte>(byte(255));  // Variable globale : pointeur feuille blanche

template<typename T>
QuadTree<T>* encodeImage(T* image, int width, int const superWidth,  int x = 0, int y = 0) {
    //Renvoi le tree de l'image encoded
    //Etape 1 : Descendre Recursivement dans les sous regions carrees de l'image
    //jusque la taille du pixel
    //Etape 2 : Une fois arrivé sur la taille du pixel, on rempli l'arbre de la valeur du pixel
    if (width == 1) {
        return new QuadLeaf<T>(image[superWidth * y + x]);
    }
    width = width/2;                                                                        //Division de l'image de base en images de largeur/hauteur egales a width/2

    QuadTree<T>* imageNW = encodeImage(image, width, superWidth, x, y);                     //Zone NW definie a partir du point en haut a gauche ie. (x,y)
    QuadTree<T>* imageNE = encodeImage(image, width, superWidth, x + width, y);             //Zone NE definie a partir du point en haut a gauche ie. (x+largeur/2,y)
    QuadTree<T>* imageSE = encodeImage(image, width, superWidth, x + width, y + width);     //Zone SE definie a partir du point en haut a gauche ie. (x+largeur/2,y+hauteur/2) or rectangle donc hauteur=largeur
    QuadTree<T>* imageSW = encodeImage(image, width, superWidth, x, y + width);             //Zone SW definie a partir du point en haut a gauche ie. (x,y+hauteur/2)

    if (equalLeaves(imageNW, imageNE, imageSE, imageSW)) {
        T val = imageNW->value();
        delete imageNW;
        delete imageNE;
        delete imageSE;
        delete imageSW;
        return new QuadLeaf<T>(val);
    }
    else {
        return new QuadNode<T>(imageNW, imageNE, imageSE, imageSW);
    }
};


template<typename T>
QuadTree<T>* quaddag(T* image, int width, int const superWidth, int x = 0, int y = 0) {
    if (width == 1) {
        if (image[superWidth * y + x] == NOIR->value()) {                       // On fait pointer tous les pixels noirs vers la feuille NOIR
            return NOIR;
        }
        else if (image[superWidth * y + x] == BLANC->value()) {                 // On fait pointer tous les pixels noirs vers la feuille BLANC
            return BLANC;
        }
    }
    width = width/2;
    QuadTree<T>* imageNW = quaddag(image, width, superWidth, x, y);
    QuadTree<T>* imageNE = quaddag(image, width, superWidth, x + width, y);
    QuadTree<T>* imageSE = quaddag(image, width, superWidth, x + width, y + width);
    QuadTree<T>* imageSW = quaddag(image, width, superWidth, x, y + width);
    imageNW->protect_leaves_from_destruction = true;
    imageNE->protect_leaves_from_destruction = true;
    imageSE->protect_leaves_from_destruction = true;
    imageSW->protect_leaves_from_destruction = true;

    if (equalLeaves(imageNW, imageNE, imageSE, imageSW)) {
        T val = imageNW->value();
        if (val == NOIR->value()) {
            return NOIR;
        }
        if (val == BLANC->value()) {
            return BLANC;
        }
    }
    else {
        QuadTree<T>* nodeTemp = new QuadNode<T>(imageNW, imageNE, imageSE, imageSW);
        nodeTemp->protect_leaves_from_destruction = true;
        return nodeTemp;
    }
};

template<typename T>
void quadtreeDecoding(QuadTree<T>* q, T* image, int width, int const superWidth, int x = 0 , int y = 0) {
    if (q->isLeaf()) {
        for (int j=x; j < x + width; j++) {
            for (int i=y; i < y + width; i++) {
                image[superWidth * i + j] = q->value();
            }
        }
    }
    else {
        width = width/2;
        quadtreeDecoding(q->son(0), image, width, superWidth, x, y);
        quadtreeDecoding(q->son(1), image, width, superWidth, x + width, y);
        quadtreeDecoding(q->son(2), image, width, superWidth, x + width, y + width);
        quadtreeDecoding(q->son(3), image, width, superWidth, x, y + width);
    }
};

int plusProchePuissance(int a){
    int b=1;
    while(a%b!=a){
        b*=2;
    }
    if (a!=(b/2)) return b;
    else return a;
};

template<typename T>
QuadTree<T>* quaddagRect(T* image, int length, int const superWidth, int const superHeight, int x = 0, int y = 0) {
    //D'abord on prend W et H de la photo et on essaye de trouver le max de W et de H.
    // Disons que c;'est H le max, alors on trouve la puissance de 2 la plus proche
    //Une fois qu'on a trouvé une puissance de 2 superieure, on a soit disant une image de taille (2N, 2N)
    //On fait le quaddag sur l'image posée en (0,0) mais dès que x>W et que y>H, pointer sur BLANC.

    //1. Ecrire une fonction qui sort le max entre deux valeurs.
    //2. Fonction qui retourne la puissance de 2 la plus proche.
    //3. Dans QuaddagNotSquare, boucle qui part de W jusque 2N, boucle qui part de H jusque2N, image[superWidth * y + x]== BLANC.value()
    //4. Quaddag (image,0,0,2N,2N)


    if (length == 1) {
        if (y>=superHeight || x>=superWidth){
            return BLANC;
        }
        else if (image[superWidth * y + x] == NOIR->value()) {                       // On fait pointer tous les pixels noirs vers la feuille NOIR
            return NOIR;
        }
        else if (image[superWidth * y + x] == BLANC->value()) {                 // On fait pointer tous les pixels noirs vers la feuille BLANC
            return BLANC;
        }
    }

    length = length/2;

    QuadTree<T>* imageNW = quaddagRect(image, length, superWidth, superHeight, x, y);
    QuadTree<T>* imageNE = quaddagRect(image, length, superWidth, superHeight, x + length, y);
    QuadTree<T>* imageSE = quaddagRect(image, length, superWidth, superHeight, x + length, y + length);
    QuadTree<T>* imageSW = quaddagRect(image, length,  superWidth, superHeight, x, y + length);
    imageNW->protect_leaves_from_destruction = true;
    imageNE->protect_leaves_from_destruction = true;
    imageSE->protect_leaves_from_destruction = true;
    imageSW->protect_leaves_from_destruction = true;

    if (equalLeaves(imageNW, imageNE, imageSE, imageSW)) {
        T val = imageNW->value();
        if (val == NOIR->value()) {
            return NOIR;
        }
        if (val == BLANC->value()) {
            return BLANC;
        }
    }
    else {
        QuadTree<T>* nodeTemp = new QuadNode<T>(imageNW, imageNE, imageSE, imageSW);
        nodeTemp->protect_leaves_from_destruction = true;
        return nodeTemp;
    }
};

template<typename T>
QuadTree<T>* compressGreyImage(T* image, int width, int const superWidth, int seuil = SEUIL, int x = 0, int y = 0) {
    //Renvoi le tree de l'image encoded en faisant la moyenne des intensités des 4 pixels
    //Etape 1 : Descendre Recursivement dans les sous regions carrees de l'image
    //jusque la taille du pixel
    //Etape 2 : Une fois arrivé sur la taille du pixel, on rempli l'arbre de la valeur du pixel
    if (width == 1) {
        return new QuadLeaf<T>(image[superWidth * y + x]);
    }

    width = width/2;                                                                        //Division de l'image de base en images de largeur/hauteur egales a width/2
    QuadTree<T>* imageNW = compressGreyImage(image, width, superWidth, seuil, x, y);                     //Zone NW definie a partir du point en haut a gauche ie. (x,y)
    QuadTree<T>* imageNE = compressGreyImage(image, width, superWidth, seuil, x + width, y);             //Zone NE definie a partir du point en haut a gauche ie. (x+largeur/2,y)
    QuadTree<T>* imageSE = compressGreyImage(image, width, superWidth, seuil, x + width, y + width);     //Zone SE definie a partir du point en haut a gauche ie. (x+largeur/2,y+hauteur/2) or rectangle donc hauteur=largeur
    QuadTree<T>* imageSW = compressGreyImage(image, width, superWidth, seuil, x, y + width);             //Zone SW definie a partir du point en haut a gauche ie. (x,y+hauteur/2)


    if (nearValueLeaves(imageNW, imageNE, imageSE, imageSW, seuil)) {
        T val = 0.25*(imageNW->value() + imageNE->value() + imageSE->value() + imageSW->value());
        seuil = seuil-3;
        delete imageNW;
        delete imageNE;
        delete imageSE;
        delete imageSW;
        return new QuadLeaf<T>(val);
    }
    else {
        return new QuadNode<T>(imageNW, imageNE, imageSE, imageSW);
    }
};


template<typename T>
bool nearValueLeaves(QuadTree<T>* t0,QuadTree<T>* t1,QuadTree<T>* t2,QuadTree<T>* t3, int seuil){
    bool res = t0->isLeaf() && t1->isLeaf() && t2->isLeaf() && t3->isLeaf();
    if (res) {
        if (((t0->value() - t1->value())<seuil) && ((t0->value() - t2->value())<seuil) && ((t0->value() - t3->value())<seuil))
            return true;
    }
    return false;
};

template<typename T>
bool equalLeaves(QuadTree<T>* t0,QuadTree<T>* t1,QuadTree<T>* t2,QuadTree<T>* t3){
    bool res = t0->isLeaf() && t1->isLeaf() && t2->isLeaf() && t3->isLeaf();
    if (res) {
        return (t0->value() == t1->value()) && (t0->value() == t2->value()) && (t0->value() == t3->value());
    }
    return res;
};



