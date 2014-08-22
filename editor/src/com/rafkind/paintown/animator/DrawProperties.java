package com.rafkind.paintown.animator;

import java.util.List;
import java.util.ArrayList;
import java.awt.Color;

public abstract class DrawProperties{
    public static interface Listener{
        public void updateBackgroundColor(Color color);
    }

    public static class Properties{
        private Color color = new Color(0, 0, 0);
        private List<Listener> listeners = new ArrayList<>();

        public void setBackgroundColor(Color newColor){
            this.color = newColor;
            for (Listener listener: listeners){
                listener.updateBackgroundColor(color);
            }
        }

        public Color getBackgroundColor(){
            return this.color;
        }

        public void addListener(Listener listener){
            this.listeners.add(listener);
        }

        public void removeListener(Listener listener){
            /* Implement? */
        }
    }

}
