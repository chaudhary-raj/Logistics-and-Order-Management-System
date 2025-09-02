# Advanced C++ Logistics & Order Management System

![Language](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

This project is a comprehensive command-line application written in C++ that simulates a sophisticated logistics and order fulfillment network. It manages operations for multiple warehouses, handling both customer take-away orders and complex home deliveries. The system's core logic is powered by advanced graph algorithms to find optimal delivery routes based on realistic, real-world road distances between major Indian cities.



---

## ## Core Features

* **Multi-Warehouse Architecture:** Simulates four independent warehouses (Delhi, Mumbai, Bengaluru, Kolkata), each with its own order queues and inventory.
* **Automatic Dispatch Routing:** For home deliveries, the system intelligently calculates and selects the **nearest warehouse** to the customer's location, ensuring the most efficient dispatch.
* **Efficient Take-Away Management:** Utilizes a self-balancing **AVL Tree** to manage take-away orders at each warehouse. This ensures O(log n) time complexity for placing, searching, and collecting orders, making it highly efficient.
* **Optimized Two-Tier Pathfinding:**
    * **Dijkstra's Algorithm:** Calculates the shortest path for **inter-city** (long-haul) deliveries from the optimal warehouse to the destination city.
    * **Prim's Algorithm:** Determines the most efficient "last-mile" route by creating a Minimum Spanning Tree (MST) for local areas **within** a major city.
* **Sequential Home Delivery Queue:** Manages home delivery orders for each warehouse using a **Linked List**, creating a straightforward FIFO (First-In, First-Out) processing queue.
* **Realistic Cost Calculation:** Delivery charges are calculated based on the total distance (inter-city + intra-city) derived from the pathfinding algorithms.
* **Robust & Safe:** Features validated user input to prevent crashes from incorrect data types and includes proper memory management to deallocate all dynamically created objects upon exit.
* **Session Order Logging:** Creates a temporary `order_log.txt` file to record all placed orders during a session, which is automatically deleted upon program termination.

---

## ## How It Works: The Logistics Logic

The system's intelligence comes from how it combines different algorithms and data structures.

### 1. Home Delivery Orders (Automated Dispatch)
1.  **Destination Input:** The user first provides their destination city.
2.  **Nearest Warehouse Calculation:** The system runs Dijkstra's algorithm from **all four warehouses** to the user's destination. It compares the results and automatically selects the warehouse that offers the shortest path.
3.  **Route Calculation:**
    * The inter-city distance is taken from the winning result of the Dijkstra calculation. The full path (e.g., `Mumbai -> Pune -> Hyderabad`) is also reconstructed and displayed.
    * If the destination is a major metropolitan area with defined local zones (like Mumbai), Prim's algorithm runs on the local city graph to find the optimal intra-city distance.
4.  **Order Queued:** The final order, with its total distance and cost, is added to the delivery queue (Linked List) of the automatically chosen warehouse.

### 2. Take-Away Orders (Manual Selection)
1.  **Warehouse Selection:** The user must manually select the specific warehouse they wish to order from and collect from.
2.  **Order Processing:** The order is added to that warehouse's **AVL Tree**, using the Order ID as the key.
3.  **Efficient Collection:** The AVL tree's balanced nature guarantees that when the user comes to collect the order, it can be found (or confirmed as non-existent) in logarithmic time, which is extremely fast even with millions of orders.

---

## ## Technology Stack

* **Primary Language:** C++
* **Core Data Structures:**
    * Graphs (Adjacency Matrix)
    * AVL Tree
    * Linked List
* **Key Algorithms:**
    * Dijkstra's Shortest Path Algorithm
    * Prim's Minimum Spanning Tree Algorithm
* **Features:**
    * File I/O (`fstream`) for logging
    * Modern C++ features (`std::vector`, validated input)

---

## ## How to Compile and Run

1.  **Prerequisites:** You need a C++ compiler like G++.
2.  **Save the Code:** Save the code as a `.cpp` file (e.g., `logistics_system.cpp`).
3.  **Compile:** Open your terminal or command prompt and run the following command:
    ```sh
    g++ -o logistics_system logistics_system.cpp -std=c++11
    ```
4.  **Execute:** Run the compiled program:
    ```sh
    ./logistics_system
    ```

---

## ## Future Enhancements

* **Graphical User Interface (GUI):** Rebuild the interface using a framework like Qt to provide a more user-friendly experience.
* **Database Integration:** Replace the in-memory data structures with a persistent database like SQLite to store orders permanently.
* **Inventory Management:** Add a system to track product stock at each warehouse, preventing users from ordering out-of-stock items.
* **Real-time Tracking Simulation:** Add timestamps to orders and simulate the delivery process by updating the order status over time.
