# CSE 232 : Assignment 1
## Command Line Utilities

Ans1. Command Used `ipconfig` 

a)![image](Images/ipconfig.jpg)

b) The IP address I see from `ipconfig` is my pc's local IP address. This is assigned by my router and is used for communication within my local network.

The IP address I see on "https://www.whatismyip.com" is my public IP address. This is the address my ISP assigned, and it's used for communication over the Internet.
![image](Images/ipconfigb.jpg)

Ans2. 

a) Command Used `nslookup -type=NS google.in`  
The above command is used to find the name servers for a domain google.in. 
![image](Images/nslookup.jpg)

To find the IP address of a individual name server, we can use the command `nslookup google.in ns1.google.com`. This will give the ipv6 and ipv4 address of the name server ns1.google.com.
![image](Images/nslookup1.jpg)

b) Command Used `nslookup -debug google.in`

- The debug mode of nslookup initially performs a reverse DNS lookup to identify the name associated with your DNS server's IP, which is why I  see a query for 1.1.1.1.in-addr.arpa and get a result of one.one.one.one.  
- For the domain "google.in", the IPv4 address is 142.250.194.164, and time to live is 259 seconds (4 minutes and 19 seconds) before my DNS server will refresh it.  
- The IPv6 address for "google.in" is 2404:6800:4002:823::2004, TTL of 300 seconds (5 minutes).

![image](Images/nslookup2.jpg)




