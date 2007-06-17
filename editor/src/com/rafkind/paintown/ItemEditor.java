package com.rafkind.paintown;

import javax.swing.*;

import com.rafkind.paintown.level.Item;
import com.rafkind.paintown.level.Level;

public class ItemEditor implements PropertyEditor {
	public ItemEditor( Item i ){
	}

	public JComponent createPane( Level level ){
		return new JPanel();
	}
}
