class Actor { 
public :
    int x,y; // position on map
    int ch; // ascii symbol code
       // a numeric value is used instead of a string literal so that 
       // symbols located outside the 'normal' range can be used
   TCODColor col; // symbol color

   Actor(int x, int y, int ch, const TCODColor &col);
   void render() const;
};
