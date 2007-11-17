package com.rafkind.paintown;

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import java.util.Iterator;

import com.rafkind.paintown.level.Cat;
import com.rafkind.paintown.level.Level;
import com.rafkind.paintown.level.Block;
import com.rafkind.paintown.level.Stimulation;

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
