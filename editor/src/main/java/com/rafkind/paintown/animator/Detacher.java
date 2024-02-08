package com.rafkind.paintown.animator;

import javax.swing.JComponent;
import javax.swing.JFrame;

interface Detacher {
    public JFrame detach(JComponent object, String name);
    public void attach(JComponent object, String name);
}
