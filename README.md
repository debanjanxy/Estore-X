# Webstore-X
An e-commerce system

Installation:
	
	Install sqlite3 in the system. Command to install sqlite3:
		sudo apt-get install sqlite3 libsqlite3-dev


	Put all the files into the same folder. Open this folder in terminal
	for 3 times. So, we have now 3 terminal windows. 

Execution:
	
	1) First, run the back-end server in one of the terminal window. Command 
	   to run are : 
				gcc back_end.c -l sqlite3
				./a.out PORT1

			Example:
				gcc back_end.c -l sqlite3
				./a.out 8080
	2) Now run the front-end server in another terminal window. Command to run:
			gcc front_end.c -l pthread
			./a.out IP_Address_of_Back_End_Server PORT1 PORT2

		Example:
			gcc front_end.c -l pthread
			./a.out 127.0.0.1 8080 8000

	3) Finally, run the client side program in the last terminal window. Also 
	   maximize your terminal window. Command to run : 
	   			gcc client.c
	   			./a.out IP_Address_of_Front_End_Server PORT2

	   		Example:
	   			gcc client.c
	   			./a.out 10.15.28.174 8000

Testing:

	You have to first log in to system from client side. 
	
	Username : test 
	Password : test
	
	Now type the item name and count of items you needed followed by a space; e.g, Laptop 2.
	Note - Please type the exact name of the item that is shown in the screen. 
	a) If item is present and number of items present in our database is greater than what was
	requested then expected output :
		
		You have to pay Rs.60000.000000 to buy 2 Laptop(s).
		Your order has been placed. Payment will be cash on delivery.

		Thanks for your interest. Visit again. Have a good day :)

	b) If item is present but requested amount of that item is greater than our capacity (e.g,
	Shampoo 100000000) then expected output:

		We don't have enough Shampoo(s).

	c) If item is not present in our database (e.g, Mouse 12) then expected output:

		Sorry, the product is not present in our store.


