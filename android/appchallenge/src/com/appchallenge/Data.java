package com.appchallenge;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class Data extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_data);
		
		final TextView TextEstado  = (TextView)findViewById(R.id.textEstado);
		final Button buttonStop = (Button)findViewById(R.id.buttonStop);
		
		final TextView TextActual  = (TextView)findViewById(R.id.textActual);
		final TextView TextCommanded  = (TextView)findViewById(R.id.textCommanded);
		final TextView TextVoltage  = (TextView)findViewById(R.id.textVoltage);
		final TextView TextCurrent  = (TextView)findViewById(R.id.textCurrent);
		
		// Implementar listeners
        OnClickListener listener_click = new OnClickListener()
        {
        	
        	public void onClick(View v){
        		
        		InputMethodManager imm = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);
        		
        		switch(v.getId())
        		{
        		case R.id.buttonStop:
        				Toast toast = Toast.makeText(getApplicationContext(),
        						"El password es alan - 123456", 
        						Toast.LENGTH_SHORT);
        				toast.show();
        				
        				break;
        			}
        		
        	}	// end onClick
        	
        };	//end listener
        
        buttonStop.setOnClickListener(listener_click);
        
	}	// end oncreate

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.data, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
}
