#include <iostream>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <string>
#include <fstream> 
#include <cstdio>  
#include <vector>
#include <limits>

using namespace std;

const int TOTAL_LOCATIONS = 11;
const int LOCAL_AREAS_COUNT = 6;
const int WAREHOUSE_COUNT = 4;
const char* LOG_FILE_NAME = "order_log.txt";

// Struct Definitions
struct Customer
{
    string name;
    string product;
    int quantity;
    double bill;

    Customer() {}

    Customer(string name, string product, int quantity, double bill)
    {
        this->name = name;
        this->product = product;
        this->quantity = quantity;
        this->bill = bill;
    }
};

struct DeliveryOrder
{
    Customer customerInfo;
    string address;
    double deliveryCharges;
    int distanceDelivery;
    DeliveryOrder *next = NULL;

    DeliveryOrder(string name, string product, int quantity, double bill, string address, double deliveryCharges, int distanceDelivery)
    {
        this->customerInfo = Customer(name, product, quantity, bill);
        this->address = address;
        this->deliveryCharges = deliveryCharges;
        this->distanceDelivery = distanceDelivery;
    }
};

struct TakeAwayOrder
{
    string name;
    string product;
    int quantity;
    double bill;
    int orderId;
    TakeAwayOrder *left;
    TakeAwayOrder *right;

    TakeAwayOrder(string name, string product, int quantity, double bill, int orderId)
    {
        this->name = name;
        this->product = product;
        this->quantity = quantity;
        this->bill = bill;
        this->orderId = orderId;
        this->left = NULL;
        this->right = NULL;
    }
};

struct Shop
{
    string name;
    string address;
    const vector<string>* products; // Using vector for modern, safer code
    const vector<int>* prices;     // Using vector
    DeliveryOrder *deliveryOrderQueue;
    TakeAwayOrder *takeAwayOrderTree;
    int cityGraphIndex;
};

// Global Variables & Data
Shop *warehouses[WAREHOUSE_COUNT];
string finalLocalAddress;
int cityDistances[TOTAL_LOCATIONS];
int localAreaDistances[LOCAL_AREAS_COUNT];
int parent[TOTAL_LOCATIONS]; 

string CityName[] = {"Delhi", "Mumbai", "Bengaluru", "Chennai", "Kolkata", "Hyderabad", "Pune", "Jaipur", "Ahmedabad", "Lucknow", "Chandigarh"};
string MumbaiAreas[] = {"Mumbai", "Andheri", "Bandra", "Dadar", "Colaba", "Juhu"};
string BengaluruAreas[] = {"Bengaluru", "Koramangala", "Indiranagar", "Jayanagar", "Whitefield", "MG Road"};
string ChennaiAreas[] = {"Chennai", "T. Nagar", "Adyar", "Anna Nagar", "Velachery", "Mylapore"};

int cityGraph[TOTAL_LOCATIONS][TOTAL_LOCATIONS] = {{0, 1420, 2150, 2215, 1530, 1580, 1460,  270,  940,  550,  250}, {1420,    0,  985, 1340, 1960,  710,  150, 1150,  525, 1370, 1665}, {2150,  985,    0,  350, 1870,  575,  840, 2100, 1520, 1930, 2400}, {2215, 1340,  350,    0, 1675,  630, 1190, 2220, 1870, 1990, 2460}, {1530, 1960, 1870, 1675,    0, 1500, 2015, 1500, 2080, 1000, 1780}, {1580,  710,  575,  630, 1500,    0,  560, 1530, 1230, 1360, 1825}, {1460,  150,  840, 1190, 2015,  560,    0, 1190,  660, 1420, 1700}, { 270, 1150, 2100, 2220, 1500, 1530, 1190,    0,  670,  580,  515}, { 940,  525, 1520, 1870, 2080, 1230,  660,  670,    0, 1050, 1180}, { 550, 1370, 1930, 1990, 1000, 1360, 1420,  580, 1050,    0,  800}, { 250, 1665, 2400, 2460, 1780, 1825, 1700,  515, 1180,  800,    0}};
int MumbaiGraph[LOCAL_AREAS_COUNT][LOCAL_AREAS_COUNT] = {{0,   12,   8,   9,  22,  10}, { 12,    0,   6,  14,  28,   5}, {  8,    6,   0,   7,  18,   9}, {  9,   14,   7,   0,  15,  16}, { 22,   28,  18,  15,   0,  25}, { 10,    5,   9,  16,  25,   0}};
int BengaluruGraph[LOCAL_AREAS_COUNT][LOCAL_AREAS_COUNT] = {{0,    6,   7,   8,  18,   5}, {  6,    0,   5,   4,  16,   8}, {  7,    5,   0,  11,  13,   4}, {  8,    4,  11,   0,  22,   9}, { 18,   16,  13,  22,   0,  16}, {  5,    8,   4,   9,  16,   0}};
int ChennaiGraph[LOCAL_AREAS_COUNT][LOCAL_AREAS_COUNT] = {{0,    5,  10,   8,  14,   9}, {  5,    0,   6,  10,  11,   4}, { 10,    6,   0,  15,   5,   5}, {  8,   10,  15,   0,  18,  12}, { 14,   11,   5,  18,   0,   8}, {  9,    4,   5,  12,   8,   0}};

// Function Definitions
void dijkstra(int graph[TOTAL_LOCATIONS][TOTAL_LOCATIONS], int distances[], int parent[], int src);

// *** NEW: Safely gets integer input from the user ***
int getValidatedInput(int min, int max) {
    int choice;
    while (true) {
        cout << "Enter your choice: ";
        cin >> choice;
        if (cin.good() && choice >= min && choice <= max) {
            // Clear the buffer in case of extra characters
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            return choice;
        }
        cin.clear(); // Clear error flags
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard bad input
        cout << "Invalid input. Please enter a number between " << min << " and " << max << "." << endl;
    }
}

pair<Shop*, int> findNearestWarehouse(int destinationIndex) {
    Shop* nearestShop = nullptr;
    int minDistance = INT_MAX;

    int tempDistances[TOTAL_LOCATIONS];
    int tempParent[TOTAL_LOCATIONS]; 

    for (int i = 0; i < WAREHOUSE_COUNT; ++i) {
        dijkstra(cityGraph, tempDistances, tempParent, warehouses[i]->cityGraphIndex);
        int currentDistance = tempDistances[destinationIndex];
        
        if (currentDistance < minDistance) {
            minDistance = currentDistance;
            nearestShop = warehouses[i];
        }
    }
    return make_pair(nearestShop, minDistance);
}

Shop* selectWarehouseManually() {
    int choice;
    cout << "\nThis action is specific to one warehouse." << endl;
    cout << "Please select a warehouse:" << endl;
    cout << "---------------------------------------" << endl;
    for (int i = 0; i < WAREHOUSE_COUNT; ++i) {
        cout << i + 1 << ". " << warehouses[i]->name << endl;
    }
    cout << "0. Go Back" << endl;
    cout << "---------------------------------------" << endl;
    choice = getValidatedInput(0, WAREHOUSE_COUNT);

    if (choice > 0) {
        return warehouses[choice - 1];
    }
    return NULL; 
}


void logOrderToFile(const string& orderDetails) {
    ofstream logFile(LOG_FILE_NAME, ios::app); 
    if (logFile.is_open()) {
        logFile << orderDetails << endl;
        logFile.close();
    } else {
        cout << "Warning: Could not open log file to record order." << endl;
    }
}

void deleteLogFile() {
    remove(LOG_FILE_NAME);
}

void printTakeAwayOrder(TakeAwayOrder *order)
{
    cout << "\n--- Take-Away Order ---" << endl;
    cout << "Order ID: " << order->orderId << endl;
    cout << "Customer Name: " << order->name << endl;
    cout << "Product: " << order->product << endl;
    cout << "Quantity: " << order->quantity << endl;
    cout << "Total Bill: " << order->bill << " INR" << endl;
    cout << "-----------------------\n" << endl;
}

TakeAwayOrder *printAllTakeAwayOrders(TakeAwayOrder *orderNode)
{
    if (orderNode)
    {
        printAllTakeAwayOrders(orderNode->left);
        printTakeAwayOrder(orderNode);
        printAllTakeAwayOrders(orderNode->right);
    }
    return orderNode;
}

bool isTakeAwayTreeEmpty(TakeAwayOrder *orderNode)
{
    return orderNode == NULL;
}

int height(TakeAwayOrder *node)
{
    if (node == NULL)
        return -1;
    else
    {
        int leftH = height(node->left);
        int rightH = height(node->right);
        return (1 + max(leftH, rightH));
    }
}

int balanceFactor(TakeAwayOrder *node)
{
    if (!node)
        return 0;
    return height(node->left) - height(node->right);
}

TakeAwayOrder *searchTakeAwayOrder(TakeAwayOrder *orderNode, int id)
{
    if (orderNode == NULL || orderNode->orderId == id)
    {
        return orderNode;
    }
    if (orderNode->orderId < id)
    {
        return searchTakeAwayOrder(orderNode->right, id);
    }
    return searchTakeAwayOrder(orderNode->left, id);
}

TakeAwayOrder *RRRotation(TakeAwayOrder *node)
{
    TakeAwayOrder *x = node->right;
    TakeAwayOrder *temp = x->left;
    x->left = node;
    node->right = temp;
    return x;
}

TakeAwayOrder *LLRotation(TakeAwayOrder *node)
{
    TakeAwayOrder *x = node->left;
    TakeAwayOrder *temp = x->right;
    x->right = node;
    node->left = temp;
    return x;
}

TakeAwayOrder *RLRotation(TakeAwayOrder *node)
{
    node->right = LLRotation(node->right);
    return RRRotation(node);
}

TakeAwayOrder *LRRotation(TakeAwayOrder *node)
{
    node->left = RRRotation(node->left);
    return LLRotation(node);
}

TakeAwayOrder *addTakeAwayOrder(Shop* currentShop, string name, int quantity, string product, double bill, int orderId, TakeAwayOrder *orderNode)
{
    if (orderNode == NULL)
    {
        stringstream ss;
        ss << "TAKEAWAY | Warehouse: " << currentShop->name << " | OrderID: " << orderId << " | Customer: " << name << " | Product: " << product << " | Qty: " << quantity << " | Bill: " << bill;
        logOrderToFile(ss.str());
        return new TakeAwayOrder(name, product, quantity, bill, orderId);
    }
    if (orderId < orderNode->orderId)
    {
        orderNode->left = addTakeAwayOrder(currentShop, name, quantity, product, bill, orderId, orderNode->left);
    }
    else if (orderId > orderNode->orderId)
    {
        orderNode->right = addTakeAwayOrder(currentShop, name, quantity, product, bill, orderId, orderNode->right);
    }
    else
    {
        cout << "Error: Duplicate Order IDs are not allowed." << endl;
        return orderNode;
    }

    int bf = balanceFactor(orderNode);

    if (bf > 1 && orderId < orderNode->left->orderId)
        return LLRotation(orderNode);
    if (bf < -1 && orderId > orderNode->right->orderId)
        return RRRotation(orderNode);
    if (bf > 1 && orderId > orderNode->left->orderId)
        return LRRotation(orderNode);
    if (bf < -1 && orderId < orderNode->right->orderId)
        return RLRotation(orderNode);

    return orderNode;
}

TakeAwayOrder *minValueNode(TakeAwayOrder *node)
{
    TakeAwayOrder *current = node;
    while (current->left != NULL)
    {
        current = current->left;
    }
    return current;
}

TakeAwayOrder *removeTakeAwayOrder(TakeAwayOrder *orderNode, int orderId)
{
    if (orderNode == NULL) return orderNode;

    if (orderId < orderNode->orderId)
        orderNode->left = removeTakeAwayOrder(orderNode->left, orderId);
    else if (orderId > orderNode->orderId)
        orderNode->right = removeTakeAwayOrder(orderNode->right, orderId);
    else
    {
        if (orderNode->left == NULL || orderNode->right == NULL)
        {
            TakeAwayOrder *temp = orderNode->left ? orderNode->left : orderNode->right;
            if (temp == NULL)
            {
                temp = orderNode;
                orderNode = NULL;
            }
            else
                *orderNode = *temp;
            delete temp;
        }
        else
        {
            TakeAwayOrder *temp = minValueNode(orderNode->right);
            orderNode->orderId = temp->orderId;
            orderNode->name = temp->name;
            orderNode->product = temp->product;
            orderNode->quantity = temp->quantity;
            orderNode->bill = temp->bill;
            orderNode->right = removeTakeAwayOrder(orderNode->right, temp->orderId);
        }
    }

    if (orderNode == NULL) return orderNode;

    int balance = balanceFactor(orderNode);

    if (balance > 1 && balanceFactor(orderNode->left) >= 0)
        return LLRotation(orderNode);
    if (balance > 1 && balanceFactor(orderNode->left) < 0)
        return LRRotation(orderNode);
    if (balance < -1 && balanceFactor(orderNode->right) <= 0)
        return RRRotation(orderNode);
    if (balance < -1 && balanceFactor(orderNode->right) > 0)
        return RLRotation(orderNode);

    return orderNode;
}

void queueDeliveryOrder(Shop* currentShop, string name, string product, int quantity, double bill, string address, int deliveryCharges, int distanceDelivery)
{
    DeliveryOrder *newDeliveryOrder = new DeliveryOrder(name, product, quantity, bill, address, deliveryCharges, distanceDelivery);

    if (currentShop->deliveryOrderQueue == NULL)
    {
        currentShop->deliveryOrderQueue = newDeliveryOrder;
    }
    else
    {
        DeliveryOrder *temp = currentShop->deliveryOrderQueue;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newDeliveryOrder;
    }
    
    stringstream ss;
    ss << "DELIVERY | Warehouse: " << currentShop->name << " | Customer: " << name << " | Product: " << product << " | Qty: " << quantity << " | Bill: " << bill << " | Address: " << address;
    logOrderToFile(ss.str());

    cout << "\n========== Home Delivery Order Confirmed ==========";
    cout << "\nDispatched from: " << currentShop->name;
    cout << "\nCustomer Name: " << name;
    cout << "\nProduct: " << product;
    cout << "\nQuantity: " << quantity;
    cout << "\nDelivery Address: " << address;
    cout << "\nTotal Distance: " << distanceDelivery << " km";
    cout << "\nDelivery Charges: " << deliveryCharges << " INR";
    cout << "\nTotal Bill: " << bill << " INR" << endl;
    cout << "===================================================\n";
}

void printAllDeliveryOrders(Shop* currentShop)
{
    if (currentShop->deliveryOrderQueue == NULL)
    {
        cout << "\nThere are no orders for home delivery at the " << currentShop->name << "." << endl;
    }
    else
    {
        cout << "\n--- All Pending Home Delivery Orders (" << currentShop->name << ") ---" << endl;
        DeliveryOrder *traversal = currentShop->deliveryOrderQueue;
        while (traversal != NULL)
        {
            cout << "------------------------------------------" << endl;
            cout << "Customer Name: " << traversal->customerInfo.name << endl;
            cout << "Product: " << traversal->customerInfo.product << endl;
            cout << "Quantity: " << traversal->customerInfo.quantity << endl;
            cout << "Delivery Address: " << traversal->address << endl;
            cout << "Distance: " << traversal->distanceDelivery << " KM" << endl;
            cout << "Charges: " << traversal->deliveryCharges << " INR" << endl;
            cout << "Total Bill: " << traversal->customerInfo.bill << " INR" << endl;
            traversal = traversal->next;
        }
        cout << "------------------------------------------" << endl;
    }
}

int minDistance(int distances[], bool visited[])
{
    int min = INT_MAX, min_index;
    for (int v = 0; v < TOTAL_LOCATIONS; v++)
        if (visited[v] == false && distances[v] <= min)
            min = distances[v], min_index = v;
    return min_index;
}

void dijkstra(int graph[TOTAL_LOCATIONS][TOTAL_LOCATIONS], int distances[], int parent[], int src)
{
    bool visited[TOTAL_LOCATIONS];
    for (int i = 0; i < TOTAL_LOCATIONS; i++) {
        distances[i] = INT_MAX;
        visited[i] = false;
        parent[i] = -1;
    }
    
    distances[src] = 0;

    for (int count = 0; count < TOTAL_LOCATIONS - 1; count++)
    {
        int u = minDistance(distances, visited);
        visited[u] = true;
        for (int v = 0; v < TOTAL_LOCATIONS; v++) {
            if (!visited[v] && graph[u][v] && distances[u] != INT_MAX && distances[u] + graph[u][v] < distances[v]) {
                distances[v] = distances[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
}

void printPath(int parent[], int j) {
    if (parent[j] == -1) { 
        cout << CityName[j];
        return;
    }
    printPath(parent, parent[j]);
    cout << " -> " << CityName[j];
}


int minKey(int distances[], bool visited[])
{
    int min = INT_MAX, min_index;
    for (int v = 0; v < LOCAL_AREAS_COUNT; v++)
        if (visited[v] == false && distances[v] < min)
            min = distances[v], min_index = v;
    return min_index;
}

void prims(int graph[LOCAL_AREAS_COUNT][LOCAL_AREAS_COUNT], int distances[])
{
    int parentMST[LOCAL_AREAS_COUNT];
    bool visited[LOCAL_AREAS_COUNT];
    for (int i = 0; i < LOCAL_AREAS_COUNT; i++)
        distances[i] = INT_MAX, visited[i] = false;

    distances[0] = 0;
    parentMST[0] = -1;

    for (int count = 0; count < LOCAL_AREAS_COUNT - 1; count++)
    {
        int u = minKey(distances, visited);
        visited[u] = true;
        for (int v = 0; v < LOCAL_AREAS_COUNT; v++)
            if (graph[u][v] && visited[v] == false && graph[u][v] < distances[v])
                parentMST[v] = u, distances[v] = graph[u][v];
    }
}

int getCityChoice()
{
    cout << "\nPlease enter your delivery city:" << endl;
    cout << "----------------------" << endl;
    cout << " 1. Mumbai      7. Jaipur" << endl;
    cout << " 2. Bengaluru   8. Ahmedabad" << endl;
    cout << " 3. Chennai     9. Lucknow" << endl;
    cout << " 4. Kolkata    10. Chandigarh" << endl;
    cout << " 5. Hyderabad   11. Delhi" << endl;
    cout << " 6. Pune" << endl;
    cout << "----------------------" << endl;
    return getValidatedInput(1, 11);
}

int getMumbaiLocation()
{
    cout << "\nPlease select your area in Mumbai:" << endl;
    cout << "-----------------" << endl;
    cout << " 1. Andheri\n 2. Bandra\n 3. Dadar\n 4. Colaba\n 5. Juhu" << endl;
    cout << "-----------------" << endl;
    return getValidatedInput(1, 5);
}

int getBengaluruLocation()
{
    cout << "\nPlease select your area in Bengaluru:" << endl;
    cout << "-----------------" << endl;
    cout << " 1. Koramangala\n 2. Indiranagar\n 3. Jayanagar\n 4. Whitefield\n 5. MG Road" << endl;
    cout << "-----------------" << endl;
    return getValidatedInput(1, 5);
}

int getChennaiLocation()
{
    cout << "\nPlease select your area in Chennai:" << endl;
    cout << "-----------------" << endl;
    cout << " 1. T. Nagar\n 2. Adyar\n 3. Anna Nagar\n 4. Velachery\n 5. Mylapore" << endl;
    cout << "-----------------" << endl;
    return getValidatedInput(1, 5);
}

int selectLocalDeliveryArea(int option)
{
    int opt;
    switch (option)
    {
    case 1:
        prims(MumbaiGraph, localAreaDistances);
        opt = getMumbaiLocation();
        finalLocalAddress = MumbaiAreas[opt];
        break;
    case 2:
        prims(BengaluruGraph, localAreaDistances);
        opt = getBengaluruLocation();
        finalLocalAddress = BengaluruAreas[opt];
        break;
    case 3:
        prims(ChennaiGraph, localAreaDistances);
        opt = getChennaiLocation();
        finalLocalAddress = ChennaiAreas[opt];
        break;
    }
    return opt;
}

// *** NEW: Safely deallocates all nodes in an AVL tree ***
void clearTakeAwayTree(TakeAwayOrder* node) {
    if (node == NULL) return;
    clearTakeAwayTree(node->left);
    clearTakeAwayTree(node->right);
    delete node;
}

// *** NEW: Safely deallocates all nodes in a linked list ***
void clearDeliveryQueue(DeliveryOrder* head) {
    while (head != NULL) {
        DeliveryOrder* temp = head;
        head = head->next;
        delete temp;
    }
}


int main()
{
    auto products = new vector<string>{"", "iPhone 16 Pro Max", "Samsung Galaxy S25 Ultra", "Google Pixel 9 Pro", "OnePlus 13", "Xiaomi 15 Pro", "Asus ROG Phone 9", "Sony Xperia 1 VI", "Samsung Galaxy Z Fold 7", "Nothing Phone (3)", "Motorola Razr+ (2025)", "iPhone 16 Plus", "Samsung Galaxy Z Flip 7", "Oppo Find X8 Pro", "Vivo X110 Pro+", "Huawei Mate 70 Pro"};
    auto prices = new vector<int>{0, 160000, 145000, 110000, 70000, 85000, 95000, 130000, 175000, 55000, 90000, 90000, 105000, 75000, 80000, 115000};
    
    warehouses[0] = new Shop{"Delhi Warehouse", "New Delhi, India", products, prices, NULL, NULL, 0};
    warehouses[1] = new Shop{"Mumbai Warehouse", "Mumbai, Maharashtra", products, prices, NULL, NULL, 1};
    warehouses[2] = new Shop{"Bengaluru Warehouse", "Bengaluru, Karnataka", products, prices, NULL, NULL, 2};
    warehouses[3] = new Shop{"Kolkata Warehouse", "Kolkata, West Bengal", products, prices, NULL, NULL, 4};

    cout << "\n=======================================================" << endl;
    cout << "      Welcome to the Automated Warehouse Order System!" << endl;
    cout << "=======================================================" << endl;

    int choice = -99;
    do
    {
        cout << "\n--- Main Menu ---\n";
        cout << "1. View Products and Prices\n";
        cout << "2. Place an Order for Warehouse Take-Away\n";
        cout << "3. Place an Order for Home Delivery\n";
        cout << "4. Collect a Take-Away Order\n";
        cout << "5. View All Home Delivery Orders (per warehouse)\n";
        cout << "6. View All Take-Away Orders (per warehouse)\n";
        cout << "0. Exit Program\n";
        
        choice = getValidatedInput(0, 6);

        Shop* currentShop = NULL; 

        string name;
        int quantity;
        int productNumber;
        double bill;
        string address;
        int orderId;
        int distance;
        int destinationIndex;
        int optionDelivery, optionDelivery1;

        switch (choice)
        {
        case 1:
        {
            size_t product_count = products->size() - 1;
            cout << "\n------------------ Our Products ------------------\n";
            cout << "No. | Item Name                 | Price (INR)\n";
            cout << "----|---------------------------|-------------\n";
            for (int i = 1; i <= product_count; ++i)
            {
                cout << setw(3) << i << " | " << left << setw(25) << (*products)[i] << " | " << (*prices)[i] << endl;
            }
            cout << "-------------------------------------------------\n";
            break;
        }
        case 2: 
        case 4: 
        case 6:
        {
            currentShop = selectWarehouseManually();
            if (currentShop == NULL) break; 

            size_t product_count = currentShop->products->size() - 1;

            if (choice == 2) {
                cout << "\n--- New Take-Away Order (" << currentShop->name << ") ---\n";
                cout << "Please enter your name: "; cin >> name;
                cout << "Enter the product number (1-" << product_count << "): ";
                productNumber = getValidatedInput(1, product_count);
                cout << "Enter the quantity: "; 
                quantity = getValidatedInput(1, 100); // Assuming max quantity of 100
                cout << "Please enter a unique numeric Order ID: "; 
                orderId = getValidatedInput(0, 99999);
                
                bill = (double)quantity * (*currentShop->prices)[productNumber];
                currentShop->takeAwayOrderTree = addTakeAwayOrder(currentShop, name, quantity, (*currentShop->products)[productNumber], bill, orderId, currentShop->takeAwayOrderTree);
                cout << "\nOrder confirmed! Your total is " << bill << " INR.\n";
                cout << "Your Order ID is: " << orderId << "\n";
                cout << "Please collect your order from the " << currentShop->name << ".\n";
            
            } else if (choice == 4) {
                 cout << "\n--- Collect Take-Away Order (" << currentShop->name << ") ---\n";
                cout << "Please enter your Order ID to collect: "; 
                orderId = getValidatedInput(0, 99999);
                TakeAwayOrder *cust = searchTakeAwayOrder(currentShop->takeAwayOrderTree, orderId);
                if (cust == NULL) { cout << "\nSorry, we couldn't find an order with that ID at this warehouse.\n"; }
                else {
                    currentShop->takeAwayOrderTree = removeTakeAwayOrder(currentShop->takeAwayOrderTree, orderId);
                    cout << "\nOrder #" << orderId << " is ready for collection. Thank you!\n";
                }
            } else { // choice == 6
                if (isTakeAwayTreeEmpty(currentShop->takeAwayOrderTree)) {
                    cout << "\nThere are no pending Take-Away orders at this warehouse.\n";
                } else {
                    cout << "\n--- All Pending Take-Away Orders (" << currentShop->name << ") ---" << endl;
                    printAllTakeAwayOrders(currentShop->takeAwayOrderTree);
                }
            }
            break;
        }
        case 3:
        {
            cout << "\n--- New Home Delivery Order ---\n";
            optionDelivery = getCityChoice();
            
            int choiceToIndexMap[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0}; 
            destinationIndex = choiceToIndexMap[optionDelivery - 1];

            pair<Shop*, int> result = findNearestWarehouse(destinationIndex);
            currentShop = result.first;
            int interCityDistance = result.second;

            if (currentShop == nullptr) {
                cout << "Sorry, we could not find a valid delivery route." << endl;
                break;
            }

            cout << "\n>>> Our system has found the nearest warehouse for you: " << currentShop->name << " <<<\n";
            
            dijkstra(cityGraph, cityDistances, parent, currentShop->cityGraphIndex);
            
            string targetCityName = CityName[destinationIndex];
            
            if (destinationIndex >= 1 && destinationIndex <= 3)
            { 
                optionDelivery1 = selectLocalDeliveryArea(destinationIndex);
                address = targetCityName + ", " + finalLocalAddress;
                distance = interCityDistance + localAreaDistances[optionDelivery1];
            }
            else
            {
                address = targetCityName;
                distance = interCityDistance;
            }

            size_t product_count = currentShop->products->size() - 1;
            cout << "\nPlease enter your name: "; cin >> name;
            cout << "Enter the product number (1-" << product_count << "): ";
            productNumber = getValidatedInput(1, product_count);
            cout << "Enter the quantity: "; 
            quantity = getValidatedInput(1, 100);

            int deliveryChargesPerKM = 25;
            int deliveryCharges = deliveryChargesPerKM * distance;
            bill = ((double)quantity * (*currentShop->prices)[productNumber]) + deliveryCharges;
            
            cout << "\nOptimal Route Calculated: ";
            printPath(parent, destinationIndex);
            cout << endl;

            queueDeliveryOrder(currentShop, name, (*currentShop->products)[productNumber], quantity, bill, address, deliveryCharges, distance);
            break;
        }
        case 5:
        {
            currentShop = selectWarehouseManually();
            if (currentShop != NULL) {
                printAllDeliveryOrders(currentShop);
            }
            break;
        }
        }
    } while (choice != 0);

    cout << "\nThank you for using our service! Have a great day.\n"
         << endl;
    
    deleteLogFile(); 

    // Cleanup
    for (int i = 0; i < WAREHOUSE_COUNT; ++i) {
        clearTakeAwayTree(warehouses[i]->takeAwayOrderTree);
        clearDeliveryQueue(warehouses[i]->deliveryOrderQueue);
        delete warehouses[i];
    }
    delete products;
    delete prices;

    return 0;
}
