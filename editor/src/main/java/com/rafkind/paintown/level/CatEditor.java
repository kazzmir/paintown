package com.rafkind.paintown.level;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import java.util.Iterator;

import com.rafkind.paintown.level.objects.Cat;
import com.rafkind.paintown.level.objects.Level;
import com.rafkind.paintown.level.objects.Block;
import com.rafkind.paintown.level.objects.Stimulation;
import com.rafkind.paintown.Lambda0;

import org.swixml.SwingEngine;

public class CatEditor implements PropertyEditor {
	
	private Cat cat;

	public CatEditor( Cat i ){
		cat = i;
	}

	public JComponent createPane( final Level level, final Lambda0 closeProc ){
		return new JPanel();
	}
}
