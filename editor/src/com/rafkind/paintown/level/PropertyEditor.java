package com.rafkind.paintown;

import javax.swing.JComponent;

import com.rafkind.paintown.level.Level;

public interface PropertyEditor{
	public JComponent createPane( Level level, Lambda0 closeProc );
}
