#include <Imagine/Graphics.h>
#include <algorithm>
#include <iostream>
#include "quadtree/quadtree.h"
#include "quadtree/encode.h"

using namespace Imagine;
using namespace std;

int main(int argc, char **argv) {
    // Get image file (default is running horse)
    const char *image_file =
        (argc > 1) ? argv[1] : srcPath("running-horse-square.png");
    // Load image
    byte* image;
    int width, height;
    cout << "Loading image: " << image_file << endl;
    loadGreyImage(image_file, image, width, height);

    // Print statistics
    /*
    cout << "Image size: " << width << "x" << height << endl;
    cout << "Number of pixels: " << width*height << endl;
    */

    // Display image
    /*
    Window window = openWindow(width, height);
    putGreyImage(IntPoint2(0,0), image, width, height);
    // Pause
    click();
    */

    //////////////////////////////
    //Encoded image
    //////////////////////////////

    QuadTree<byte>* encodedHorse= encodeImage(image, width, width);

    //display(encodedHorse);

    //////////////////////////////
    //Quaddag
    //////////////////////////////

    QuadTree<byte>* qaddag= quaddag(image, width, width);
    //cout<<qaddag->nNodes()<<endl;

    //display(qaddag);

    //////////////////////////////
    //Image decoding
    //////////////////////////////

    byte* imageDecoded = new byte[width * height];
    quadtreeDecoding(qaddag, imageDecoded, width, width);

    // Display decoded image
/*
    Window window1 = openWindow(width, height);
    putGreyImage(IntPoint2(0,0), imageDecoded, width, height);
    // Pause
    click();
*/


    //////////////////////////////
    //Pour une image rectangulaire
    //////////////////////////////
    const char *image_file1 =
        (argc > 1) ? argv[1] : srcPath("running-horse-rect.png");
    // Load image
    byte* image1;
    int width1, height1;
    loadGreyImage(image_file1, image1, width1, height1);

    int length = plusProchePuissance(max(height1, width1));
    cout << "Loading image: " << image_file1 << endl;
    //cout << "Image size: " << width1 << "x" << height1 << endl;
    //cout << "Puissance de 2 la plus proche " << length << endl;

    QuadTree<byte>* rectEncoding= quaddagRect(image1, length, width1, height1);
    //display(rectEncoding);


    //////////////////////////////
    //Pour une image grise
    //////////////////////////////
    const char *image_file2 =
        (argc > 1) ? argv[1] : srcPath("testimage-2.jpg");
    // Load image
    byte* image2;
    int width2, height2;
    loadGreyImage(image_file2, image2, width2, height2);

    QuadTree<byte>* encodedGreyImage= compressGreyImage(image2, width2, width2);

    // image decoding
    byte* imageDecoded1 = new byte[width2 * height2];
    quadtreeDecoding(encodedGreyImage, imageDecoded1, width2, width2);

    // Display decoded image
    Window window2 = openWindow(width2, height2);
    putGreyImage(IntPoint2(0,0), imageDecoded1, width2, height2);
    // Pause
    click();


    delete encodedHorse;
    delete qaddag;
    delete rectEncoding;
    delete encodedGreyImage;

    delete[] imageDecoded;
    delete[] imageDecoded1;

    return 0;
}
