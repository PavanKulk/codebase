# codebase
hostapd :-
 - Initialize hostapd
 - Register get_params_handler for every 5 seconds
 - Once the timer is triggered: 
    - Access the required parameters using eloop context and put those parameters in a file
 - Note: Changes implemented in hostapd/hostapd/main.c file

httpserver :-
 - Import required http libraries
 - Start the daemon using MHD_start_daemon function by adding answer_to_connection as the call back function
 - Once the http request arrives
    - If it is a GET request:
      - Read the file contents and add the contents into a buffer
      - Generate HTTP reponse and add the response in the message queue
 - Note: Changes implemented in httpserver/libmicrohttpd-0.9.16/myhttpserver/myhttpserver.c 

