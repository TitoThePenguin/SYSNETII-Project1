/*
 * TCPclient.java
 * Systems and Networks II
 * Project 1
 *
 * This file describes the functions to be implemented by the TCPclient class
 * You may also implement any auxillary functions you deem necessary.
 */

public class TCPclient
{
	private Socket _socket; // the socket for communication with a server
	
	/**
	 * Constructs a TCPclient object.
	 */
	public TCPclient()
	{
	}
	
	/**
	 * Creates a streaming socket and connects to a server.
	 *
	 * @param host - the ip or hostname of the server
	 * @param port - the port number of the server
	 *
	 * @return - 0 or a negative number describing an error code if the connection could not be established
	 */
	public int createSocket(String host, int port)
	{
		
	}

	/**
	 * Sends a request for service to the server. Do not wait for a reply in this function. This will be
	 * an asynchronous call to the server.
	 * 
	 * @request - the request to be sent
	 *
	 * @return - 0, if no error; otherwise, a negative number indicating the error
	 */
	public int sendRequest(String request)
	{
		
	}
	
	/**
	 * Receives the server's response. Also displays the response in a
	 * clear and concise format.
	 *
	 * @return - the server's response or NULL if an error occured
	 */
	public String receiveResponse()
	{
		
	}
	
	/*
    * Prints the response to the screen in a formatted way.
    *
    * response - the server's response as an XML formatted string
    *
    */
	public static void printResponse(String response)
	{
	
	}
 

	/*
	 * Closes an open socket.
	 *
    * @return - 0, if no error; otherwise, a negative number indicating the error
	 */
	public int closeSocket() 
	{
		
	}

	/**
	 * The main function. Use this function for 
	 * testing your code. We will use our own main function for testing.
	 */
	public void main(String[] args)
	{
		
	}

}
