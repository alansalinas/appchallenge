package com.appchallenge;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.charset.Charset;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.os.AsyncTask;

public class MainActivity extends Activity {
	
	private static final char READ_STATUS = 0x31;
	//private static final char READ_PARAMS = 0x32;
	private static final char START_MOTOR = 0x33;
	private static final char STOP_MOTOR = 0x34;
	private static final char SET_SPEED = 0x35;
	
	private UIUpdater mUIUpdater;
	
	
	
	//private Socket socket;
	private TCPClient mTcpClient;
	
	private TextView textEstado;
	
	private TextView textActual;
	private TextView textCommanded;
	private TextView textVoltage;
	private TextView textCurrent;


	@Override
	protected void onCreate(Bundle savedInstanceState) {
		
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		
		textEstado  = (TextView)findViewById(R.id.textEstado);
		final Button buttonStop = (Button)findViewById(R.id.buttonStop);
		
		final Button buttonStart = (Button)findViewById(R.id.buttonStart);
		
		final EditText editSpeed = (EditText)findViewById(R.id.editSpeed);
		final Button buttonSetSpeed = (Button)findViewById(R.id.buttonSetSpeed);
		
		textActual  = (TextView)findViewById(R.id.textActual);
		textCommanded  = (TextView)findViewById(R.id.textCommanded);
		textVoltage  = (TextView)findViewById(R.id.textVoltage);
		textCurrent  = (TextView)findViewById(R.id.textCurrent);
		
		//new Thread(new ClientThread()).start();
		textEstado.setText("Comm off");
		// connect to the server
        new connectTask().execute("");
        textEstado.setText("Comm");
		
		// Implementar listeners
        OnClickListener listener_click = new OnClickListener()
        {
        	
        	public void onClick(View v){
        		
        		InputMethodManager imm = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
        		
        		switch(v.getId())
        		{
        		case R.id.buttonSetSpeed:
        				
        				//sends the message to the server
                        if (mTcpClient != null) {
                        	int rpm = Integer.parseInt(editSpeed.getText().toString());
                        	int hz = (rpm*10) / 60;
                        	char[] buf = new char[4];
                        	
                        	buf[0] = SET_SPEED;
                        	buf[1] = (char) ((hz & 0xff00) >> 8);
                        	buf[2] = (char) (hz & 0x00ff);
                        	buf[3] = '\n';
                        	System.out.println("DEBUG (SET_SPEED): " + Integer.toString(hz));
                            mTcpClient.sendStream(buf, 0, 3);
                        }
                        //imm.hideSoftInputFromWindow(getCurrentFocus().getWindowToken(), 0);
        				
        				
        				break;
        				
        		case R.id.buttonStop:
        			//sends the message to the server
                    if (mTcpClient != null) {
                    	char[] buf = new char[2];
                    	buf[0] = STOP_MOTOR;
                        mTcpClient.sendStream(buf, 0, 1);
                        System.out.println("BUTTON_STOP\n");
                    }
                    //imm.hideSoftInputFromWindow(getCurrentFocus().getWindowToken(), 0);
        			break;
        			
        		case R.id.buttonStart:
        			if (mTcpClient != null) {
                    	char[] buf = new char[2];
                    	buf[0] = START_MOTOR;
                        mTcpClient.sendStream(buf, 0, 1);
                        System.out.println("BUTTON_START\n");
                    }
        			break;
        			
        			}
        		
        	}	// end onClick
        	
        };	//end listener
        
        buttonStop.setOnClickListener(listener_click);
        buttonStart.setOnClickListener(listener_click);
        buttonSetSpeed.setOnClickListener(listener_click);
        
    	mUIUpdater = new UIUpdater(new Runnable() {
            @Override 
            public void run() {
               // do stuff ...
            	//System.out.println("DEBUG RUN\n");
            	if (mTcpClient != null) {
                	char[] buf = new char[2];
                	buf[0] = READ_STATUS;
                    mTcpClient.sendStream(buf, 0, 1);
                }
            }
       });
    	
    	mUIUpdater.startUpdates();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.menu_estado) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
	

	
	/*
	 * Funciones de acceso a UI desde thread de red
	 */
	
	public void setEstado(int stat, int feed, int cmd, int volt, int curr) {	// despliega estado del variador
	    final int estado = stat;
	    final int feedback = feed;
	    final int commanded = cmd;
	    final int voltage = volt;
	    final int current = curr;
	    
	    System.out.println("ESTADO:" + Integer.toString(estado) + "\n");
	    System.out.println("FEEDBACK:" + Integer.toString(feedback) + "\n");
	    System.out.println("COMMANDED:" + Integer.toString(commanded) + "\n");
	    System.out.println("CURRENT:" + Integer.toString(current) + "\n");
	    System.out.println("VOLTAGE:" + Integer.toString(voltage) + "\n");
	    
	    runOnUiThread(new Runnable() {
	        @Override
	        public void run() {
	        	
	        	if ( (estado & 0x0010) > 0)
	        	{
	        		textEstado.setText("Acelerando");
	        	}else if ( (estado & 0x0020) > 0)
	        	{
	        		textEstado.setText("Desacelerando");
	        	}
	        	else if ( (estado & 0x0100) > 0)
	        	{
	        		textEstado.setText("Estable");
	        	}else if((estado & 0x0002) > 0)
	        	{
	        		textEstado.setText("Activo");
	        	}else
	        	{
	        		textEstado.setText("Na");
	        	}
	        	
	        	textActual.setText( Integer.toString((feedback * 600) / 100) + " rpm");
	        	textCommanded.setText( Integer.toString((commanded * 600) / 100) + " rpm");
	        	textVoltage.setText(Integer.toString(voltage));
	        	textCurrent.setText(Integer.toString(current));
	        	
	        	
	        }	// end run
	    });
	}

	
	/*
	 * Clase interna para manejar conexiones de red asincronamente
	 */
	
	public class connectTask extends AsyncTask<String,String,TCPClient> {
		 
        @Override
        protected TCPClient doInBackground(String... message) {
 
            //we create a TCPClient object and
            mTcpClient = new TCPClient(new TCPClient.OnMessageReceived() {
                @Override
                //here the messageReceived method is implemented
                public void messageReceived(byte[] message) {
                    //this method calls the onProgressUpdate
                    //publishProgress(message);
                	
                	int c1 = (int)(message[1]) & 0x00FF;
                	int c2 = (int)(message[2]) & 0x00FF;
                	int c3 = (int)(message[3]) & 0x00FF;
                	int c4 = (int)(message[4]) & 0x00FF;
                	int c5 = (int)(message[5]) & 0x00FF;
                	int c6 = (int)(message[6]) & 0x00FF;
                	int c7 = (int)(message[7]) & 0x00FF;
                	int c8 = (int)(message[8]) & 0x00FF;
                	int c9 = (int)(message[9]) & 0x00FF;
                	int c10 = (int)(message[10]) & 0x00FF;
                	
            	
            		System.out.println(Integer.toString(c5));
            		System.out.println(Integer.toString(c6));
                	switch(message[0]){
                	case READ_STATUS:
                		System.out.println("RECIEVE STATUS\n");
                		int stat = 0;
                		// obtener estado
                		stat = (c1 *256) + c2;
                		
                		// obtener feedback
                		int feedback = 0;
                		feedback = (c3 *256) + c4;
                		
                		// obtener velocidad referida
                		int commanded = 0;
                		commanded = (c5 *256) + c6;
                		
                		int voltage = 0;
                		voltage = (c7 *256) + c8;
                		
                		int current = 0;
                		current = (c9 *256) + c10;;
                		
                		setEstado(stat, feedback, commanded, voltage, current);
                		break;
                		
                		default:
                			break;
                		
                	}
                }
            });
            mTcpClient.run();
 
            return null;
        }
 
    }

	
	
}	// END ACTIVITY
