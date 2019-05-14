
import java.io.*;
import java.lang.*;
import java.util.regex.*;
import java.net.*;
import gnu.getopt.Getopt;
import java.math.BigInteger;
import javax.jws.WebService;
import javax.xml.namespace.QName;
import com.dataaccess.webservicesserver.*;

class Multithread extends Thread
{
  //Class for creating a thread to listen the notifications
  ServerSocket ssc;
  String thread_topic;
    //Constructor to pass the server socket and the topic
    public Multithread (ServerSocket sc, String topic){
        ssc=sc;
        thread_topic=topic;
    }

    public void run(){
        try{
          BigInteger number;
          NumberConversion myservice=new NumberConversion();
          NumberConversionSoapType port=myservice.getNumberConversionSoap();
          String response=null;
          Integer number_;
          Integer length;
          //Accepting all the notifications from the broker and printing them
          String line=null;
            while(true){//
              Socket sc=ssc.accept();
              BufferedReader in= new BufferedReader(new InputStreamReader(sc.getInputStream()));
              while((line=in.readLine())!=null){
              line = line.trim().replaceAll(" +", " ");//double spaces are removed
              //numbers are found and converted to words using the web service
              Pattern p=Pattern.compile("\\d+");
              Matcher m = p.matcher(line);
              while(m.find()) {
                number_=Integer.valueOf(m.group());
                number=BigInteger.valueOf(number_);
                response=port.numberToWords(number);
                response=response.substring(0,(response.length()-1));
                line=line.replaceAll(m.group(),m.group()+" ("+response+")");
              }

              System.out.println("MESSAGE FROM <"+thread_topic+"> : <" +line+">");
            }
          }

        }
          catch(IOException e){}
      }
}

public class suscriptor{

	/********************* TYPES **********************/

	static Socket sc;
	static BufferedOutputStream s;
	static DataInputStream istream;

	/******************* ATTRIBUTES *******************/

	private static String _server   = null;
	private static short _port = -1;
	private static String action = null;
	static int res=-1;


	/********************* METHODS ********************/

	static int subscribe(String topic)
	{
    ServerSocket ssc;
    int sc_port=0;
		res=1;
    BufferedReader instring=null;
		topic=topic+'\0';
    //Connection with the broker is established and the Thread is created.
		try{
			sc=new Socket(_server,_port);
			s=new BufferedOutputStream(sc.getOutputStream());
			istream=new DataInputStream(sc.getInputStream());
      instring= new BufferedReader(new InputStreamReader(sc.getInputStream()));
      ssc= new ServerSocket(0);
      sc_port=ssc.getLocalPort();

  		Multithread object = new Multithread(ssc,topic);
      object.start();
	}
		catch(Exception e){
		System.out.print("Error in the connection to the server <"+_server+">:<"+_port+">\n");
		System.exit(0);
	}
    //The action, the topic and the ServerSocket of the thread are sent. Then, the result is read
		try{
			action="SUBSCRIBE"+'\0';
      String portbuf=String.valueOf(sc_port)+'\0';

			s.write(action.getBytes());
			s.flush();
			s.write(topic.getBytes());
			s.flush();
      s.write(portbuf.getBytes());
      s.flush();
			res=istream.readInt();
	}
		catch(Exception e){
		System.out.print("NETWORK ERROR\n");
	}
		//0 if OK, 1 if fails
		if(res==0){

      String line=null;
      try{
        line=instring.readLine();
        if(line != null && !line.isEmpty()){
          System.out.println("SUBSCRIBE OK: Last text from <"+topic+"> : <" +line+">");
      }
    }
      catch(Exception e){
  		System.out.print("NETWORK ERROR\n");
  	}

	}
		else System.out.println("SUBSCRIBE FAIL");
		return res;
	}

	static int unsubscribe(String topic)
	{
		res=1;
		topic=topic+'\0';
		//Connection with the broker is established.
		try{
			sc=new Socket(_server,_port);
			s=new BufferedOutputStream(sc.getOutputStream());
			istream=new DataInputStream(sc.getInputStream());
	}
		catch(Exception e){
		System.out.print("Error in the connection to the broker <"+_server+">:<"+_port+">\n");
		System.exit(0);
	}
  //The action and the topic are sent. Then, the result is read
		try{
			action="UNSUBSCRIBE"+'\0';
			s.write(action.getBytes());
			s.flush();
			s.write(topic.getBytes());
			s.flush();
			res=istream.readInt();
	}
	catch(Exception e){
	System.out.print("NETWORK ERROR\n");
}
		//0 if OK, 1 if topic doesnt exist, 2 if fails
		if(res==0)System.out.println("UNSUBSCRIBE OK");
		else if(res==2) System.out.println("UNSUBSCRIBE FAIL");
		else System.out.println("TOPIC NOT SUBSCRIBED");

		return res;
	}



	/**
	 * @brief Command interpreter for the suscriptor. It calls the protocol functions.
	 */
	static void shell()
	{
		boolean exit = false;
		String input;
		String [] line;
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

		while (!exit) {
			try {
				System.out.print("c> ");
				input = in.readLine();
				line = input.split("\\s");

				if (line.length > 0) {
					/*********** SUBSCRIBE *************/
					if (line[0].equals("SUBSCRIBE")) {
						if  (line.length == 2) {
							subscribe(line[1]); // topic = line[1]
						} else {
							System.out.println("Syntax error. Usage: SUBSCRIBE <topic>");
						}
					}

					/********** UNSUBSCRIBE ************/
					else if (line[0].equals("UNSUBSCRIBE")) {
						if  (line.length == 2) {
							unsubscribe(line[1]); // topic = line[1]
						} else {
							System.out.println("Syntax error. Usage: UNSUBSCRIBE <topic>");
						}
                    }

                    /************** QUIT **************/
                    else if (line[0].equals("QUIT")){
						if (line.length == 1) {
							exit = true;
						} else {
							System.out.println("Syntax error. Use: QUIT");
						}
					}

					/************* UNKNOWN ************/
					else {
						System.out.println("Error: command '" + line[0] + "' not valid.");
					}
				}
			} catch (java.io.IOException e) {
				System.out.println("Exception: " + e);
				e.printStackTrace();
			}
		}
	}

	/**
	 * @brief Prints program usage
	 */
	static void usage()
	{
		System.out.println("Usage: java -cp . suscriptor -s <server> -p <port>");
	}

	/**
	 * @brief Parses program execution arguments
	 */
	static boolean parseArguments(String [] argv)
	{
		Getopt g = new Getopt("suscriptor", argv, "ds:p:");

		int c;
		String arg;

		while ((c = g.getopt()) != -1) {
			switch(c) {
				//case 'd':
				//	_debug = true;
				//	break;
				case 's':
					_server = g.getOptarg();
					break;
				case 'p':
					arg = g.getOptarg();
					_port = Short.parseShort(arg);
					break;
				case '?':
					System.out.print("getopt() returned " + c + "\n");
					break; // getopt() already printed an error
				default:
					System.out.print("getopt() returned " + c + "\n");
			}
		}

		if (_server == null)
			return false;

		if ((_port < 1024) || (_port > 65535)) {
			System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
			return false;
		}

		return true;
	}



	/********************* MAIN **********************/

	public static void main(String[] argv)
	{
		if(!parseArguments(argv)) {
			usage();
			return;
		}

    //Shell is initialised
		shell();


		System.out.println("+++ FINISHED +++");
	}
}
