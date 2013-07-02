/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

package com.havok.Vision;

import android.app.Activity;
import android.os.Bundle;
import android.view.Display;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.text.InputType;

public class VirtualKeyboardActivity extends Activity  {
	
	Button cancel;
	Button accept;
	EditText edit;
	
	public static native void setNativeText(String textValue);
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

		try {
			// Load native library
			String libName = getCallingActivity().getPackageName();
			System.loadLibrary(libName.substring(libName.lastIndexOf(".") + 1));

			// Create UI stuff
			ScrollView sv = new ScrollView(this);
			LinearLayout ll0 = new LinearLayout(this);
			ll0.setOrientation(LinearLayout.VERTICAL);
			sv.addView(ll0);
        
			edit = new EditText(this);
			ll0.addView(edit);
        
			// Get data provided by native side
			Bundle extras = getIntent().getExtras();
			if (extras != null) {
        
        		setTitle(extras.getString("EDIT_TEXT_TITLE"));
        		edit.setText(extras.getString("EDIT_TEXT_VALUE"));
				edit.setSelection(edit.getText().length());
				
				if (extras.getString("EDIT_TEXT_PASSWORD") != null)
					edit.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
			}
        
			LinearLayout ll1 = new LinearLayout(this);
			ll1.setOrientation(LinearLayout.HORIZONTAL);
			ll0.addView(ll1);
        
			accept = new Button(this);
			accept.setText("Accept");
			accept.setLayoutParams(new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT, 1.0f));
			accept.setOnClickListener(new View.OnClickListener() {

				@Override
				public void onClick(View v) {
					setNativeText(edit.getText().toString());
					finish();
				}
			});
			ll1.addView(accept);
        
			cancel = new Button(this);
			cancel.setText("Cancel");
			cancel.setLayoutParams(new LinearLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT, 1.0f));
			cancel.setOnClickListener(new View.OnClickListener() {

				@Override
				public void onClick(View v) {
					finish();
				}
			});
			ll1.addView(cancel);
        
			this.setContentView(sv);
		} catch (Exception e) {
			System.out.println(e);
		}
    }
    
    @Override
    public void onAttachedToWindow() {
        super.onAttachedToWindow();
        
        Display display = getWindowManager().getDefaultDisplay();

        View view = getWindow().getDecorView();
        WindowManager.LayoutParams lp = (WindowManager.LayoutParams) view.getLayoutParams();
        lp.gravity = Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL;
        lp.width = display.getWidth() - 80;
        lp.height = LayoutParams.WRAP_CONTENT;
        getWindowManager().updateViewLayout(view, lp);
    }
	
}

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
