/* steiner.cpp
 *  Graphics demo program for getting started with QtCreator for program 6, Steiner Trees.
 *
 * Date: 11/27/17
 * Class: CS 141, Fall 2017
 * Lab: Tues 9am
 * TA: Moumita Samanta
 * System: QtCreator with Stanford graphics library, on Windows
 * Author: Clark Chen
 *
 */

#include <iostream>
#include "gobjects.h"  // for all graphics objects
#include "gevents.h"   // for mouse events
#include <fstream>        // For file input
#include <cmath>
#include <ctime>
using namespace std;

// Global constants
const int PointRadius = 7;

// Create a graphics window, which will be used to draw graphical shapes.
// This can be a global variable, but you may not have any other global variables.
GWindow graphicsWindow;

//------------------------------------------------------------------------------------------
//  Class for button
class MyButton{
    public:
        MyButton(){
            posX = 0;
            posY = 0;
            objectWidth = 0;
            objectHeight = 0;
            objectText = "";
            initObject();
        }

        MyButton(int inputX, int inputY, int inputW, int inputH, string inputColor, string inputText){
            posX = inputX;
            posY = inputY;
            objectWidth = inputW;
            objectHeight = inputH;
            objectColor = inputColor;
            objectText = inputText;
            initObject();
        }

        // Function for initialize the object
        void initObject(){

            // Initialize button object
            int newButtonX = posX;
            int newButtonY = posY;
            int newButtonWidth = objectWidth;
            int newButtonHeight = objectHeight;
            rectObject = new GRect(newButtonX, newButtonY, newButtonWidth, newButtonHeight);
            rectObject->setColor( objectColor);
            rectObject->setFilled( true);
            graphicsWindow.add( rectObject);

            // Initialize label object
            labelObject = new GLabel();
            labelObject->setText(objectText);
            // Find size of text, to determine placement relative to enclosing rectangle
            int xOffset = (newButtonWidth  - labelObject->getBounds().getWidth()) / 2;
            int yOffset = (newButtonHeight - labelObject->getBounds().getHeight() + (labelObject->getBounds().getHeight() / 2));
            labelObject->setLocation( newButtonX + xOffset, newButtonY + yOffset);
            graphicsWindow.add( labelObject);
        }

        // function return true if x, y is within object
        bool posIsInObject(int inputX, int inputY){
            bool isInObject = rectObject->contains(inputX, inputY);
            return isInObject;
        }

    private:
        int posX; // Position coordinate X
        int posY; // Position coordinate Y
        int objectWidth; // Object's width
        int objectHeight; // Object's Height
        string objectColor; // Object's Color
        string objectText; // The text show on the object
        GRect *rectObject; // button(rect) object
        GLabel *labelObject; // label Object
};
//------------------------------------------------------------------------------------------
// Dot(Point) Class
class MyDot{
    public:
        MyDot(){
            posX = 0;
            posY = 0;
            initObject();
        }

        MyDot(int inputX, int inputY){
            posX = inputX;
            posY = inputY;
            initObject();
        }

        // Function for initialize the object
        // Optional parameter for whether showing the object after init (default not showing)
        void initObject(bool showObject=false){
            ovalObject = new GOval( posX-PointRadius, posY-PointRadius, PointRadius*2, PointRadius*2);
            ovalObject->setFilled( true);
            ovalObject->setColor("blue");
            ovalObject->setVisible( showObject);
            graphicsWindow.add(ovalObject);
        }

        // Set position of the object
        // Take in position x, y, and output (optional: show object, default showing)
        void setPos(int inputX, int inputY, bool showObject=true){
            posX = inputX;
            posY = inputY;
            initObject(showObject);
        }

        // Set color of the object
        // Take in color in string
        void setColor(string color){
            ovalObject->setColor(color);
        }

        // Get current Object's position coordinate X, return in int
        int getPosX(){
            return posX;
        }

        // Get current Object's position coordinate Y, return in int
        int getPosY(){
            return posY;
        }

        // function return true if x, y within object
        // Take in x, y, return true if position is in object, else return false
        bool posIsInObject(int inputX, int inputY){
            bool isInObject = ovalObject->contains(inputX, inputY);
            return isInObject;
        }

    private:
        int posX; // Position coordinate X
        int posY; // Position coordinate Y
        GOval *ovalObject; // Dot Object
};

//------------------------------------------------------------------------------------------
// Line class
class MyLine{
    public:
        MyLine(){
            beginX = 0;
            beginY = 0;
            endX = 0;
            endY = 0;
            initObject();
        }
        MyLine(int x1, int y1, int x2, int y2){
            beginX = x1;
            beginY = y1;
            endX = x2;
            endY = x2;
            initObject();
        }

        // Function for initialize the object
        void initObject( ){
            lineObject = new GLine(beginX, beginY, endX, endY);
            lineObject->setVisible(false);
            lineObject->setLineWidth(2);
            graphicsWindow.add(lineObject);
        }

        // Set the start point of this line
        void setStartPoint(int x1, int y1){
            beginX = x1;
            beginY = y1;
            lineObject->setStartPoint(x1, y1);
        }

        // Set the end point of this line
        void setEndPoint(int x2, int y2){
            endX = x2;
            endY = y2;
            lineObject->setEndPoint(x2, y2);
        }

        // Set the visibility of the line
        void setVisible(bool visibility){
            lineObject->setVisible(visibility);
        }

        // Get the begin coordinate X
        int getBeginX(){
            return beginX;
        }

        // Get the begin coordinate Y
        int getBeingY(){
            return beginY;
        }

        // Get the end coordinate X
        int getEndX(){
            return endX;
        }

        // Get the end coordinate Y
        int getEndY(){
            return endY;
        }

        // Get the length of this line
        int getLength(){
            return sqrt( pow(endY-beginY, 2) + pow(endX-beginX, 2));
        }

    private:
        // Begin coordinate
        int beginX;
        int beginY;

        // End Coordinate
        int endX;
        int endY;

        GLine *lineObject; // Object for line
};

// Calculate distance between 2 point
int calcDist(int x1, int y1, int x2, int y2){
    return sqrt( pow(y2-y1, 2) + pow(x2-x1, 2));
}

//------------------------------------------------------------------------------------------
// Read in the sets of points information from the data file
// You muse make the pointsArray Vector a reference parameter so that read-in values are
// reflected back to the caller.
void readInputFile( MyDot (&dotObjectArray)[100], int &howManyPoints, int &idealScore)
{
    ifstream inputFileStream;  // declare the input file stream

    // Open input file and verify.
    // You must first:
    //    1. Outside of QtCreator go into your project's resources folder, copy readme.txt into data.txt
    //    2. Edit data.txt to have the datafile information as shown on the assignment page
    //    3. Now WITHIN QtCreator right-click on the "res" folder (under "Other files") and
    //       choose "add new" and add the new data.txt file so it shows up in your environment.
    inputFileStream.open("data.txt");
    if( !inputFileStream.is_open()) {
        cout << "Could not find input file data.txt.  Exiting..." << endl;
        exit( -1);
    }

    // Read sets of data from input file.  Each read operation reads the next set of data.
    // The first line in the input file indicates which set to use.
    int whichSetToUse;
    inputFileStream >> whichSetToUse;

    int x, y;             // Store individual point x,y values from input file
    // Reserve some reasonable number of total points for the vector, so that
    //dotObjectArray.ensureCapacity( 10);

    // Read in and discard some number of sets of points before we get the real set we will be using
    for( int i = 0; i < whichSetToUse-1; i++) {
        inputFileStream >> howManyPoints >> idealScore;
        for( int j = 0; j < howManyPoints; j++) {
            inputFileStream >> x >> y;    // Read in the points
        }
    }

    inputFileStream >> howManyPoints >> idealScore;

    // Read in and store the points information
    for( int i = 0; i < howManyPoints; i++) {
        inputFileStream >> x >> y;    // Read in the points
        // This is where you should take the points information and create points on the screen.
        // For now we will just print out the points information.
        cout << "Point " << i << " is at " << x << "," << y << endl;
        dotObjectArray[i].setPos(x, y);
        dotObjectArray[i].setColor("black");
    }
}//end readInputFile()

//------------------------------------------------------------------------------------------
// Function for update the current length label
void updateCurrentLength(GLabel *lengthLabel, int newLength){
    char message[81];
    sprintf( message,"%d", newLength);
    lengthLabel->setText(message);
}// end void updateCurrentLength()

//------------------------------------------------------------------------------------------
// Process run before each mouse action
bool universalActionForMode(int mouseX, int mouseY, char currentEventType, char &currentMode,
                            MyButton &addPointsButton, MyButton &drawLinesButton, MyButton &exitButton,
                            GLabel *messagesLabel)
{
    bool returnToMain = false; // Default not return to main
    switch (currentEventType) {

    // If Press
    case 'P':

        // Check for press on Add Dot button
        if (addPointsButton.posIsInObject(mouseX, mouseY)){
            currentMode = 'D';
            returnToMain = true;
        }

        // Check for press on Draw Line button
        else if (drawLinesButton.posIsInObject(mouseX, mouseY))
        {
            currentMode = 'L';
            returnToMain = true;
        }

        // Check for press on exit button
        else if (exitButton.posIsInObject(mouseX, mouseY))
        {
            currentMode = 'E';
            returnToMain = true;
        }
        break;

    // If Release
    case 'R':

        // Prevent Action at release
        if (addPointsButton.posIsInObject(mouseX, mouseY)){
            returnToMain = true;
        }
        else if (drawLinesButton.posIsInObject(mouseX, mouseY))
        {
            returnToMain = true;
        }
        else if (exitButton.posIsInObject(mouseX, mouseY))
        {
            returnToMain = true;
        }
        break;

    // If Drag
    case 'D':
        //Prevent Action When drag
        if (addPointsButton.posIsInObject(mouseX, mouseY)){
            returnToMain = true;
        }
        else if (drawLinesButton.posIsInObject(mouseX, mouseY))
        {
            returnToMain = true;
        }
        else if (exitButton.posIsInObject(mouseX, mouseY))
        {
            returnToMain = true;
        }

        cout << "Mouse dragged to " << mouseX << "," << mouseY << endl;
        break;
    }

    // Return bool whether return to Main
    return returnToMain;
}// end bool universalActionForMode()

//------------------------------------------------------------------------------------------
// Function for draw Line mode
void drawLineMode(int mouseX, int mouseY, char currentEventType, char &currentMode,
                  MyButton &addPointsButton, MyButton &drawLinesButton, MyButton &exitButton,
                  GLabel *messagesLabel, MyDot (&dotObjectArray)[100], int &currentDotIndex, MyLine (&lineObjectArray)[100], int &currentLineIndex,
                  GLabel *lengthLabel, int &currentLineTotalLength, int idealScore)
{
    // Run universal action, like detect click point, if return false continue, if return true go back to main
    if (universalActionForMode(mouseX, mouseY, currentEventType, currentMode,
                           addPointsButton, drawLinesButton, exitButton,
                               messagesLabel))
    {
        return;
    }

    int closestPointIndex = -1; // Store closest Point's index
    int closestPointDist = 9999; // Store closest point's distance
    char message[ 81]; // Message container


    // Find the cloest Point
    for (int i = 0; i < currentDotIndex; i++){
        // determine the closest point

        if (calcDist(dotObjectArray[i].getPosX(), dotObjectArray[i].getPosY(), mouseX, mouseY) < closestPointDist){
            closestPointDist = calcDist(dotObjectArray[i].getPosX(), dotObjectArray[i].getPosY(), mouseX, mouseY);
            closestPointIndex = i;
        }
    }

    switch (currentEventType) {
        // If Press
        case 'P':
            // create the initial point of the line
            lineObjectArray[currentLineIndex].setStartPoint(dotObjectArray[closestPointIndex].getPosX(), dotObjectArray[closestPointIndex].getPosY());
            lineObjectArray[currentLineIndex].setEndPoint(mouseX, mouseY);
            lineObjectArray[currentLineIndex].setVisible(true);

            // Update header message when the add line process initiated on click
            sprintf( message, "Settling line starting point to %d,%d",
                     dotObjectArray[closestPointIndex].getPosX(), dotObjectArray[closestPointIndex].getPosY());  // print into a string
            messagesLabel->setText(message);

            break;

        // If release
        case 'R':
            // Set the end point of the line
            lineObjectArray[currentLineIndex].setEndPoint(dotObjectArray[closestPointIndex].getPosX(), dotObjectArray[closestPointIndex].getPosY());

            // Add the new line length into the total line length, and update the display value
            currentLineTotalLength += lineObjectArray[currentLineIndex].getLength();
            updateCurrentLength(lengthLabel, currentLineTotalLength);

            // Update header message to when line process finish, and detect if the current line length is in the ideal range or not.
            if (currentLineTotalLength <= idealScore*1.05){
                sprintf( message, "Great! %d is less or close to %d", currentLineTotalLength, idealScore);  // print into a string
            }
            else
            {
                sprintf( message, "Sorry, %d is larger than %d", currentLineTotalLength, idealScore);  // print into a string
            }
            messagesLabel->setText(message);

            // Switch the next array index, after the current line completely created
            currentLineIndex++;

            break;

        // If drag
        case 'D':
            // Update the end point of the line as the mouse move
            lineObjectArray[currentLineIndex].setEndPoint(mouseX, mouseY);
            updateCurrentLength(lengthLabel, currentLineTotalLength+lineObjectArray[currentLineIndex].getLength());
            break;
    }

}//end void drawLineMode()

//------------------------------------------------------------------------------------------
// Process for add points mode
void addPointsMode(int mouseX, int mouseY, char currentEventType, char &currentMode,
                   MyButton &addPointsButton, MyButton &drawLinesButton, MyButton &exitButton,
                   GLabel *messagesLabel, MyDot (&dotObjectArray)[100], int &currentDotIndex)
{
    // Run universal action, like detect click point, if return false continue, if return true go back to main
    if (universalActionForMode(mouseX, mouseY, currentEventType, currentMode,
                           addPointsButton, drawLinesButton, exitButton,
                               messagesLabel))
    {
        return;
    }

    switch (currentEventType) {
    // If Press
    case 'P':
        // Add a point at the point where the mouse pressed
        dotObjectArray[currentDotIndex++].setPos(mouseX, mouseY);
    }

}//end void addPointsMode()

//------------------------------------------------------------------------------------------
int main()
{
    char currentMode = 'L'; // Current Program Mode: L for Line, D for Dot, E for Exit, O for Other(else)
    char currentEventType = '?'; // P for Press, R for release, D for drag
    int currentLineIndex = 0;
    int currentDotIndex = 0;
    int currentLineTotalLength = 0;
    graphicsWindow.setCanvasSize( 400, 400);   // Set the size
    graphicsWindow.setWindowTitle("  schen237  QtCreator Program 6");

    int mouseX = -1;                // Store mouse coordinate X
    int mouseY = -1;                // Store mouse coordinate Y
    char message[ 81];              // Used to construct message to be displayed at top of window
    int idealScore = 0;             // Precomputed ideal score for each puzzle

    MyDot dotObjectArray[100]; // Store all points from player input
    MyLine lineObjectArray[100]; // Store all lines from player input

    // Get the set of data to use from the input file
    readInputFile( dotObjectArray, currentDotIndex, idealScore);

    //// Create the Control and message components.
    // Create the messages label at the top of the window
    GLabel *messagesLabel = new GLabel();
    messagesLabel->setText("Find the minimal spanning tree, using additional points.");
    messagesLabel->setLocation( 30,20);
    graphicsWindow.add( messagesLabel);  // As with all components, after creating them you must also
                                         // add them to the graphics window, otherwise they will
                                         // exist but won't show up.

    // Create Length Display
    GLabel *lengthTitleLabel = new GLabel();
    lengthTitleLabel->setText("Length:");
    lengthTitleLabel->setLocation(330, 360);
    graphicsWindow.add(lengthTitleLabel);
    GLabel *lengthLabel = new GLabel();
    sprintf( message,"%d", currentLineTotalLength);
    lengthLabel->setText(message);
    lengthLabel->setLocation(340, 380);
    graphicsWindow.add(lengthLabel);

    // Control Button
    MyButton exitButton(250, 350, 50, 30, "red", "Exit");
    MyButton addPointsButton(10, 350, 70, 30, "green", "Add Points");
    MyButton drawLinesButton(90, 350, 70, 30, "lightgray", "Draw Lines");

    graphicsWindow.requestFocus();

    GMouseEvent e;      // Stores the mouse event each time through the event loop


    // Main event loop, where handle all the event
    while (currentMode != 'E') {
        // Wait for a mouse event, then get the x,y coordinates
        e = waitForEvent(MOUSE_EVENT);
        // Get the current mouse x,y location
        mouseX = e.getX();
        mouseY = e.getY();

        // Record Action
        switch (e.getEventType()) {
        case MOUSE_PRESSED:
            currentEventType = 'P';
            break;
        case MOUSE_RELEASED:
            currentEventType = 'R';
            break;
        case MOUSE_DRAGGED:
            currentEventType = 'D';
            break;
        default:
            // "O"ther mode won't ever happen, unless program error
            currentEventType = 'O';
            break;
        }

        // Job dispatcher, decide what process should run in current Mode
        switch (currentMode) {
        // If in "D"ot (add points) Mode
        case 'D':
            addPointsMode(mouseX, mouseY, currentEventType, currentMode,
                          addPointsButton, drawLinesButton, exitButton,
                          messagesLabel, dotObjectArray, currentDotIndex);
            break;

        // If in "L"ine drawing mode
        case 'L':
            drawLineMode(mouseX, mouseY, currentEventType, currentMode,
                         addPointsButton, drawLinesButton, exitButton,
                         messagesLabel, dotObjectArray, currentDotIndex, lineObjectArray, currentLineIndex,
                         lengthLabel, currentLineTotalLength, idealScore);
            break;
        }

    }//end while( true)

    // Close the windows, first the graphics window
    graphicsWindow.requestFocus();
    graphicsWindow.close();
    exitGraphics();

    return 0;
}

