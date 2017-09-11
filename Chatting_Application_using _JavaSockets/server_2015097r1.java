package cn_as2;
import java.util.*;
import java.io.*;
import java.net.*;

public class server_2015097r1 {
	
	public static void main(String args[]) throws IOException
	{
		HashMap<Integer,Socket> cdata = new HashMap<Integer,Socket>();
		
		ServerSocket ss=null;
		try {
			ss = new ServerSocket(1234);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		int client_count = 0;
		while(true)
		{
			client_count++;
			final int cc = client_count;
			Socket s=ss.accept();
			cdata.put(cc, s);
			System.out.println("Client "+ client_count+ " connected!");
			DataInputStream ips=new DataInputStream(s.getInputStream());;
			DataOutputStream ops=new DataOutputStream(s.getOutputStream());;
			for(int i=0;i<10;i++){} 
			Thread msg = new Thread(new Runnable(){
				
				public void run() {
					// TODO Auto-generated method stub
					
					while(true)
					{
						try {
							String val = ips.readUTF();
							
							System.out.println("Recieved from Client " + cc + " :" + val);
							
							StringTokenizer st = new StringTokenizer(val,":");
							
							String firsthalf = st.nextToken();
							String secondhalf = "";
							
							if(st.hasMoreTokens())
							{
								secondhalf = st.nextToken();
							}
							
							if(val.equals("exit"))
							{
								ops.writeUTF("exitclient");
								s.close();
								// need to remove the client from the hashmap
								cdata.remove(cc);
								System.out.println("Client " + cc + " disconnected!");
								break;
							}
							
							if(firsthalf.equals("All"))
							{
								for(Map.Entry m: cdata.entrySet())
								{
									Socket sn = (Socket) m.getValue();
									DataOutputStream opsn=new DataOutputStream(sn.getOutputStream());;
									opsn.writeUTF(secondhalf);
								}
							}
							
							else if(firsthalf.equals("Server"))
							{
								String names = "";
								for(Map.Entry m : cdata.entrySet())
								{
									names  = names + m.getKey() + " ";
								}
								
								/*for(Map.Entry m : cdata.entrySet())
								{
									Socket sn = (Socket) m.getValue();
									DataOutputStream opsn=new DataOutputStream(sn.getOutputStream());;
									opsn.writeUTF(names);
								}*/
								
								ops.writeUTF(names);
								
								
							}
							
							else
							{
								StringTokenizer st2 = new StringTokenizer(firsthalf," ");
								
								String who = st2.nextToken();
								String where = st2.nextToken();
								
								//System.out.println("who: "+who+" where: "+where);
								
								StringTokenizer st3 = new StringTokenizer(where,",");
								
								String X = st3.nextToken();
								String Y = "";
								
								//System.out.println("X: "+ X);
								
								//System.out.println(cdata);
								
								int y = 0;
								boolean valy = false;
								if(st3.hasMoreTokens())
								{
									Y = st3.nextToken();
									y = Integer.parseInt(Y);
									valy = true;
								}
								
								//System.out.println("Y: "+ Y);
								
								int x = Integer.parseInt(X);
								
								if(cdata.containsKey(x))
								{
									//System.out.println(x);
									Socket sn = cdata.get(x);
									DataOutputStream opsn=new DataOutputStream(sn.getOutputStream());;
									opsn.writeUTF(secondhalf);
									
								}
								
								else
								{
									ops.writeUTF("No Client " + X);
								}
								
								if(valy)
								{
									if(cdata.containsKey(y))
									{
										Socket sn = cdata.get(y);
										DataOutputStream opsn=new DataOutputStream(sn.getOutputStream());;
										opsn.writeUTF(secondhalf);
									}
									
									else
									{
										ops.writeUTF("No Client " + Y);
									}
								}
								
							}
								
							
						} catch (IOException e) {
							// TODO Auto-generated catch block
							//System.out.println("1111111111111111111111111");
							e.printStackTrace();
						}
						
					}
					
					try {
						ips.close();
						ops.close();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
						//System.out.println("22222222222222222222222");
					}		
					
				}
				
			});

			msg.start();
			
		}		
		
	}
	
}


