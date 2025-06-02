#include <QuickDraw.h>
#include <Events.h>
#include <Fonts.h>
#include <Windows.h>
#include <Menus.h>
#include <ToolUtils.h>
#include <ctype.h>
#include <string.h>
#include <Serial.h>
#include <Devices.h>
#include <OSUtils.h>
#include <Memory.h>
#include <stdio.h>
#include <stdlib.h>
#include "qr_data.h"

// Screen dimensions and layout constants
#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 342
#define LIST_ITEM_HEIGHT 20
#define COLUMN_WIDTH 150
#define MAX_NAMES 165
#define ALPHABET_COUNT 26

// NameEntry struct definition
typedef struct
{
    int number;
    char *firstname;
    char *lastname;
} NameEntry;

// Function prototypes
unsigned char *c2pstrb(const char *cstr);
void drawAlphabet(WindowPtr winPtr, char selectedLetter);
void drawNames(WindowPtr winPtr, NameEntry names[], int nameCount, int rowsPerColumn, int columnsPerPage);
int filterNames(NameEntry allNames[], int totalNames, NameEntry filteredNames[], char letter);
void showPopup(WindowPtr parentWin, NameEntry selectedName);
void showPrintingPopup(WindowPtr parentWin, NameEntry selectedName);
void drawCenteredString(WindowPtr winPtr, const char *str, int areaWidth, int y);
void drawButton(WindowPtr winPtr, int x, int y, int width, int height, const char *text);
void sendData(const char *data, long len);
void printBitmap(const unsigned char *bmpData, int width, int height);
void printCard(NameEntry selectedName);

// Global variables
const char subsetLetters[] = {'A', 'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'R', 'S', 'T', 'V', 'W'};
const int subsetCount = sizeof(subsetLetters) / sizeof(subsetLetters[0]);
const int cornerWidth = 12;  // Width of the rounded corner arcs
const int cornerHeight = 12; // Height of the rounded corner arcs

int randomCounter = 0;
const char *funFacts[5] = {
    "Fun Fact: Charlie's middle name is Rico\r\nbecause he was born in Puerto Rico\r\n",
    "Fun Fact: We didn't know June 14th was\r\nFlag Day when we were planning\r\n",
    "Fun Fact: Brian and Melissa met in their\r\nfirst week of college in Oswego in 2014\r\n",
    "Fun Fact: Melissa loves recommending\r\nproducts. Ask her about Kindle Scribe,\r\nHatch, or her Memory Foam Pillow\r\n",
    "Fun Fact: Brian doesn't like\r\nketchup or mustard\r\n"};

// Printer Driver reference number
short refNum;

// Main function
void main()
{
    // Window setup
    WindowRecord winRecord;
    WindowPtr winPtr;
    Rect winRect;
    EventRecord event;
    MenuHandle fileMenu;
    Boolean running = true;

    // List of names
    NameEntry names[MAX_NAMES] = {{3, "Michael", "Abrams"}, {7, "Jeff", "Allder"}, {7, "Shannon", "Allder"}, {10, "Kevin", "Baron"}, {5, "Joey", "Bednarski"}, {5, "Lisa", "Bednarski"}, {12, "Charlie", "Belekis"}, {12, "Margaret", "Belekis"}, {5, "Max", "Beutel"}, {10, "Javier", "Caamano"}, {10, "Kathleen", "Caamano"}, {9, "Chrissy", "Ciampaglione"}, {9, "Vinny", "Ciampaglione"}, {1, "Danny", "Ciaramella"}, {5, "Ryan", "Conklin"}, {11, "Jack", "Coppola"}, {1, "Trevor", "Davies"}, {10, "Madison", "Delapenha"}, {10, "Robert", "Delapenha"}, {10, "Samantha", "Delapenha"}, {14, "Ann", "DelMastro"}, {14, "Chris", "DelMastro"}, {14, "Frank", "DelMastro"}, {1, "Nick", "DelMastro"}, {14, "Patricia", "DelMastro"}, {14, "Theresa", "DelMastro"}, {13, "John", "Demarco"}, {15, "Matt", "DePaul"}, {15, "Quiana", "DePaul"}, {8, "John", "Dorsey"}, {10, "Julie", "Dorsey"}, {8, "Kevin", "Dorsey"}, {2, "Lois", "Dorsey"}, {8, "Mary Ellen", "Dorsey"}, {10, "Michael", "Dorsey (CHI)"}, {1, "Michael", "Dorsey Jr"}, {2, "Michael", "Dorsey Sr"}, {1, "Samantha", "Dorsey"}, {13, "Megan", "Dowd"}, {8, "Aileen", "Dupree"}, {13, "Nicole", "Farmer"}, {7, "Greg", "Ferese"}, {7, "Kim", "Ferese"}, {5, "Danielle", "Ferrer"}, {14, "Lorraine", "Fichera"}, {14, "Vincent", "Fichera"}, {4, "Thea", "Filo"}, {3, "Hannah", "Fitzpatrick"}, {5, "Mark", "Forcello"}, {6, "Christina", "Galow"}, {6, "Josh", "Galow"}, {17, "Chris", "Gannon"}, {3, "Jason", "Gavigan"}, {13, "Corey", "Gittleman"}, {2, "Karin", "Glenski"}, {2, "Randy", "Glenski"}, {13, "Leanna", "Governale"}, {12, "Eloise", "Grieco"}, {15, "Lewis", "Haskell"}, {15, "TinaMarie", "Haskell"}, {8, "Carolyn", "Henretta"}, {9, "Sherry", "Hicks"}, {15, "Ryan", "Johnson"}, {16, "Daniel", "Kaye"}, {16, "Laura", "Kaye"}, {16, "Alex", "Kellenberger"}, {16, "Alex", "Kellenberger"}, {16, "Jody", "Kellenberger"}, {11, "Stephen", "Kelley"}, {9, "Dan", "Kelly"}, {18, "James", "Kelly"}, {18, "Robin", "Kelly"}, {3, "Shannon", "Kelly"}, {9, "Sue", "Kelly"}, {6, "Alexandros", "Kerwick"}, {4, "Dave", "Kerwick"}, {4, "Despina", "Kerwick"}, {6, "Georgia", "Kerwick"}, {6, "Pano", "Kerwick"}, {8, "Letitia", "Khan"}, {4, "Vikie", "Kozyrakis"}, {4, "Yianni", "Kozyrakis"}, {1, "Joseph", "LaBonte"}, {2, "Joseph", "Labonte Grampa"}, {2, "Nia", "LaBonte"}, {2, "Rick", "LaBonte"}, {2, "Roslyn", "Labonte"}, {3, "Jabroni", "Leone"}, {15, "Michael", "Levano"}, {7, "Kara", "Lollar"}, {16, "James", "Lyle"}, {16, "Taylor", "Lyle"}, {11, "Kayla", "MacDermott"}, {11, "Ethan", "Maguire"}, {18, "Allen", "Malden"}, {18, "Cathy", "Malden"}, {12, "Alex", "Mans"}, {15, "Ryan", "Martin"}, {4, "Linda", "McAteer"}, {8, "Thomas", "McCarthy"}, {18, "Carol", "McGuirk"}, {18, "Liam", "McGuirk"}, {3, "Ben", "Mulson"}, {3, "Emily", "Mulson"}, {1, "Rachel", "Neal"}, {5, "Stephanie", "Nichols"}, {15, "Flynn", "O'Connell"}, {7, "Brendan", "O'Dell"}, {7, "Chris", "O'Dell"}, {7, "Mike", "O'Dell"}, {12, "Jim", "Olsen"}, {12, "Terese", "Olsen"}, {10, "Emily", "Penuen"}, {10, "Kyle", "Penuen"}, {8, "Sheila", "Penuen"}, {15, "Bella", "Petrucci"}, {11, "Amanda", "Pfitscher"}, {3, "Michael", "Reitsch"}, {9, "Anita", "Robinson"}, {9, "Eddie", "Robinson"}, {18, "Herb", "Roff"}, {18, "Kerry", "Roff"}, {9, "MaryAnn", "Rogers"}, {9, "Paul", "Rogers"}, {3, "Shelby", "Rogers"}, {6, "Eleni", "Romanelli"}, {6, "Tony", "Romanelli"}, {11, "Joe", "Samaan"}, {17, "Ann", "Sandford"}, {17, "Bob", "Sandford"}, {1, "Lauren", "Sandford"}, {17, "Paul", "Sandford"}, {17, "Rachel", "Sandford"}, {17, "Zach", "Sandford"}, {11, "Dalton", "Sargent"}, {15, "Karl", "Schroeder"}, {16, "Kenneth", "See"}, {16, "Sarah", "See"}, {13, "Dan", "Stadt"}, {13, "Pasqualina", "Stadt"}, {11, "Tom", "Stritt"}, {5, "Taylor", "Sullivan"}, {5, "Jake", "Tamburro"}, {5, "Jordan", "Tannenbaum"}, {11, "Emily", "Tekel"}, {11, "Rebecca", "Thomas"}, {12, "Tom", "Todd"}, {17, "Gina", "Tyler"}, {18, "Gordon", "Tyler"}, {18, "Sue", "Tyler"}, {4, "Charlie", "Vakirtzis"}, {12, "Fran", "Vakirtzis"}, {4, "Katerina", "Vakirtzis"}, {12, "Montos", "Vakirtzis"}, {6, "Panagiotis", "Vakirtzis"}, {13, "Sabrina", "Volino"}, {1, "Claire", "Wajda"}, {11, "Caitlin", "Whalen"}, {13, "Emily", "Williams"}, {15, "Jenna", "Winter"}};

    // Initialize system components
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitCursor();
    InitMenus();

    // Create the menu bar
    fileMenu = NewMenu(1, "\pFile");
    AppendMenu(fileMenu, "\pQuit/Q");
    InsertMenu(fileMenu, 0);
    DrawMenuBar();

    // Set the window bounds to cover the entire screen
    SetRect(&winRect, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Create a fullscreen window
    winPtr = (WindowPtr)NewWindow(&winRecord, &winRect, "\pName List", true, plainDBox, (WindowPtr)-1L, false, 0);
    FlushEvents(everyEvent, 0);

    // Variables for filtering and displaying names
    char selectedLetter = '\0'; // No letter selected initially
    NameEntry filteredNames[MAX_NAMES];
    int filteredCount = MAX_NAMES;
    Boolean needsRedraw = true;

    // Main event loop
    while (running)
    {
        // Redraw the screen if necessary
        if (needsRedraw)
        {
            EraseRect(&winPtr->portRect);
            drawAlphabet(winPtr, selectedLetter);

            if (selectedLetter != '\0')
            {
                filteredCount = filterNames(names, MAX_NAMES, filteredNames, selectedLetter);
                drawNames(winPtr, filteredNames, filteredCount, 8, 3);
            }
            else
            {
                drawNames(winPtr, names, MAX_NAMES, 8, 3);
            }

            needsRedraw = false;
        }

        // Handle events
        if (WaitNextEvent(everyEvent, &event, 0, NULL))
        {
            long menuResult = MenuSelect(event.where);
            if (HiWord(menuResult) == 1 && LoWord(menuResult) == 1)
            {
                running = false;
            }

            if (event.what == mouseDown)
            {
                Point mousePoint = event.where;
                GlobalToLocal(&mousePoint);

                // Handle alphabet clicks
                if (mousePoint.v >= 30 && mousePoint.v < 40)
                {
                    int totalWidth = subsetCount * 20;
                    int startX = (SCREEN_WIDTH - totalWidth) / 2;
                    int letterIndex = (mousePoint.h - startX) / 20;
                    if (letterIndex >= 0 && letterIndex < subsetCount)
                    {
                        char newSelectedLetter = subsetLetters[letterIndex];
                        if (newSelectedLetter != selectedLetter)
                        {
                            selectedLetter = newSelectedLetter;
                            needsRedraw = true;
                        }
                    }
                }
                // Handle name clicks
                else if (mousePoint.v >= 44)
                {
                    int rowsPerColumn = 8;
                    int column = (mousePoint.h - 20) / COLUMN_WIDTH;
                    int row = (mousePoint.v - 40) / LIST_ITEM_HEIGHT;
                    int nameIndex = column * rowsPerColumn + row;

                    if (nameIndex >= 0 && nameIndex < (selectedLetter != '\0' ? filteredCount : MAX_NAMES))
                    {
                        NameEntry selectedName = (selectedLetter != '\0') ? filteredNames[nameIndex] : names[nameIndex];
                        showPopup(winPtr, selectedName);
                        needsRedraw = true;
                    }
                }
            }
        }
    }
}

// Helper functions (organized by functionality)

// Draw the alphabet at the top of the screen
void drawAlphabet(WindowPtr winPtr, char selectedLetter)
{
    SetPort(winPtr);
    EraseRect(&winPtr->portRect);

    int totalWidth = subsetCount * 20;
    int startX = (SCREEN_WIDTH - totalWidth) / 2;

    for (int i = 0; i < subsetCount; i++)
    {
        int x = startX + i * 20;
        int y = 40;
        MoveTo(x, y);
        char letter[2] = {subsetLetters[i], '\0'};
        if (selectedLetter == letter[0])
        {
            TextFace(bold);
        }
        DrawString(c2pstrb(letter));
        TextFace(normal);
    }
}

// Draw the names in columns
void drawNames(WindowPtr winPtr, NameEntry names[], int nameCount, int rowsPerColumn, int columnsPerPage)
{
    int nameIndex = 0;
    for (int col = 0; col < columnsPerPage; col++)
    {
        int x = 20 + col * COLUMN_WIDTH;
        int y = 60;
        for (int row = 0; row < rowsPerColumn && nameIndex < nameCount; row++)
        {
            MoveTo(x, y);
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "%s %s", names[nameIndex].firstname, names[nameIndex].lastname);
            DrawString(c2pstrb(buffer));
            y += LIST_ITEM_HEIGHT;
            nameIndex++;
        }
    }
}

// Popup functions
void showPopup(WindowPtr parentWin, NameEntry selectedName)
{
    Boolean popupRunning = true;
    Rect popupRect;
    EventRecord popupEvent;
    WindowRecord popupWinRecord;
    WindowPtr popupWinPtr;

    int popupWidth = 300;
    int popupHeight = 120;

    SetRect(&popupRect, 100, 100, 100 + popupWidth, 100 + popupHeight); // Fixed position for simplicity
    popupWinPtr = (WindowPtr)NewWindow(&popupWinRecord, &popupRect, "\pWELCOME", true, rDocProc, (WindowPtr)-1L, false, 0L);
    // Draw a simple rectangle on the parent window
    SetPort(popupWinPtr);

    char fullnameBuffer[256];
    snprintf(fullnameBuffer, sizeof(fullnameBuffer), "%s %s", selectedName.firstname, selectedName.lastname);

    TextSize(20);
    TextFont(2);
    drawCenteredString(popupWinPtr, fullnameBuffer, popupWidth, 25);

    TextSize(12);
    TextFont(0);
    drawCenteredString(popupWinPtr, "you will be sitting at", popupWidth, 50);

    char tableBuffer[128];
    snprintf(tableBuffer, sizeof(tableBuffer), "%s %d", "Table ", selectedName.number);

    TextSize(18);
    TextFont(2);
    drawCenteredString(popupWinPtr, tableBuffer, popupWidth, 75);

    // Define the "Print" button rectangle
    Rect printButtonRect;
    SetRect(&printButtonRect, 220, 90, 300, 115); // x, y, width, height
    drawButton(popupWinPtr, printButtonRect.left, printButtonRect.top, 70, 25, "print");

    // Define the "Close" button rectangle
    Rect closeButtonRect;
    SetRect(&closeButtonRect, 10, 90, 90, 115); // x, y, width, height
    drawButton(popupWinPtr, closeButtonRect.left, closeButtonRect.top, 70, 25, "back");

    // Event loop for the popup window
    while (popupRunning)
    {
        if (WaitNextEvent(everyEvent, &popupEvent, 0, NULL))
        {
            if (popupEvent.what == mouseDown)
            {
                Point mousePoint = popupEvent.where;
                GlobalToLocal(&mousePoint); // Convert global coordinates to local window coordinates

                // Check if the click is inside the "Print" button
                if (PtInRect(mousePoint, &printButtonRect))
                {
                    // Show the printing waiting screen
                    showPrintingPopup(popupWinPtr, selectedName);
                    popupRunning = false;
                }
                // Check if the click is inside the "Close" button
                else if (PtInRect(mousePoint, &closeButtonRect))
                {
                    popupRunning = false;
                }
            }
        }
    }

    CloseWindow(popupWinPtr);
    SetPort(parentWin);
}

void showPrintingPopup(WindowPtr parentWin, NameEntry selectedName)
{
    Boolean popupRunning = true;
    Rect popupRect;
    EventRecord popupEvent;
    WindowRecord popupWinRecord;
    WindowPtr popupWinPtr;

    int popupWidth = 300;
    int popupHeight = 120;

    SetRect(&popupRect, 100, 100, 100 + popupWidth, 100 + popupHeight);
    popupWinPtr = (WindowPtr)NewWindow(&popupWinRecord, &popupRect, "\pPrinting", true, rDocProc, (WindowPtr)-1L, false, 0L);
    // Draw a simple rectangle on the parent window
    SetPort(popupWinPtr);

    TextSize(18);
    TextFont(0);
    drawCenteredString(popupWinPtr, "Printing", popupWidth, 40);

    TextSize(12);
    TextFont(0);
    drawCenteredString(popupWinPtr, "please wait...", popupWidth, 60);

    printCard(selectedName);

    CloseWindow(popupWinPtr); 
    SetPort(parentWin);
}

// Printing functions
void printCard(NameEntry selectedName)
{
    OSErr err = OpenDriver("\p.AOut", &refNum);

    // Set baud to 9600, 8N1
    short config = baud9600 | data8 | noParity | stop10;
    err = SerReset(refNum, config);

    // --- HEADER ---
    // Center alignment
    const char centerAlign[] = {0x1B, 'a', 1};
    sendData(centerAlign, sizeof(centerAlign));

    const char date[] = "June 14, 2025\r\n";
    sendData(date, strlen(date));

    // Double-size (GS ! 0x11)
    const char quadSize[] = {0x1D, '!', 0x33};
    sendData(quadSize, sizeof(quadSize));

    const char fontA[] = {0x1B, 'M', 0}; // ESC M 0
    sendData(fontA, sizeof(fontA));

    // Bold on (ESC E 1)
    const char boldOn[] = {0x1B, 'E', 1};
    sendData(boldOn, sizeof(boldOn));

    const char welcomeHeader[] = "WELCOME\r\n\r\n";
    sendData(welcomeHeader, strlen(welcomeHeader));

    // Reset double-size (GS ! 0)
    const char normalSize[] = {0x1D, '!', 0};

    const char doubleSize[] = {0x1D, '!', 0x11};
    sendData(doubleSize, sizeof(doubleSize));

    // --- SUBTITLE ---
    // Keep bold on, normal size
    char guestName[128];
    snprintf(guestName, sizeof(guestName), "%s %s\r\n\r\n", selectedName.firstname, selectedName.lastname);
    sendData(guestName, strlen(guestName));

    // Turn off bold
    const char boldOff[] = {0x1B, 'E', 0};
    sendData(boldOff, sizeof(boldOff));

    sendData(doubleSize, sizeof(doubleSize));
    // --- Table Number ---
    char tableNumber[128];
    snprintf(tableNumber, sizeof(tableNumber), "Table %d\r\n\r\n", selectedName.number);
    sendData(tableNumber, strlen(tableNumber));

    sendData(normalSize, sizeof(normalSize));

    const char *funFact = funFacts[randomCounter];
    sendData(funFact, strlen(funFact));
    randomCounter++;
    if (randomCounter == 5)
    {
        randomCounter = 0;
    }

    // QR CODE
    printBitmap(qr_bin, 200, 200);

    const char qrInfo[] = "Share your photos with us!\r\n";
    sendData(qrInfo, strlen(qrInfo));

    // Feed 5 lines to push paper
    const char feed[] = {0x1B, 'd', 5};
    sendData(feed, sizeof(feed));

    const char cut[] = {0x1D, 'V', 0};
    sendData(cut, sizeof(cut));

    // Close driver
    CloseDriver(refNum);
}

// Utility functions
int filterNames(NameEntry allNames[], int totalNames, NameEntry filteredNames[], char letter)
{
    int count = 0;
    for (int i = 0; i < totalNames; i++)
    {
        if (toupper(allNames[i].lastname[0]) == toupper(letter))
        {
            filteredNames[count++] = allNames[i];
        }
    }
    return count;
}

unsigned char *c2pstrb(const char *cstr)
{
    static unsigned char pstr[256];
    size_t len = strlen(cstr);
    pstr[0] = (unsigned char)len;
    memcpy(&pstr[1], cstr, len);
    return pstr;
}

void drawCenteredString(WindowPtr winPtr, const char *str, int areaWidth, int y)
{
    SetPort(winPtr);
    unsigned char *pstr = c2pstrb(str);
    int stringWidth = TextWidth(pstr, 1, pstr[0]);
    int x = (areaWidth - stringWidth) / 2;
    MoveTo(x, y);
    DrawString(pstr);
}

void drawButton(WindowPtr winPtr, int x, int y, int width, int height, const char *text)
{
    SetPort(winPtr);
    Rect borderRect;
    SetRect(&borderRect, x, y, x + width, y + height);
    FrameRoundRect(&borderRect, 10, 10);
    unsigned char *pstr = c2pstrb(text);
    int textWidth = TextWidth(pstr, 1, pstr[0]);
    int textX = (borderRect.left + borderRect.right - textWidth) / 2;
    int textY = (borderRect.top + borderRect.bottom) / 2 + 6;
    MoveTo(textX, textY);
    DrawString(pstr);
}

void sendData(const char *data, long len)
{
    long count = len;
    OSErr err = FSWrite(refNum, &count, data);
    if (err != noErr)
    {
        printf("FSWrite failed: %d\n", err);
    }
}

void printBitmap(const unsigned char *bmpData, int width, int height)
{
    int widthBytes = (width + 7) / 8;
    unsigned char xL = widthBytes & 0xFF;
    unsigned char xH = (widthBytes >> 8) & 0xFF;
    unsigned char yL = height & 0xFF;
    unsigned char yH = (height >> 8) & 0xFF;
    const char rasterHeader[] = {0x1D, 'v', '0', 0, xL, xH, yL, yH};
    sendData(rasterHeader, sizeof(rasterHeader));
    long count = widthBytes * height;
    OSErr err = FSWrite(refNum, &count, (const char *)bmpData);
    if (err != noErr)
    {
        printf("FSWrite failed (bitmap): %d\n", err);
    }
}