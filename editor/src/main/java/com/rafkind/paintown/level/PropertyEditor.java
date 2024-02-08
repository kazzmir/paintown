package com.rafkind.paintown.level;

import javax.swing.JComponent;

import com.rafkind.paintown.level.objects.Level;
import com.rafkind.paintown.Lambda0;

public interface PropertyEditor{
	public JComponent createPane( Level level, Lambda0 closeProc );
}
