# include <Plot.h>

Circle::Circle()
{
}

Circle::~Circle()
{
}

Circle::Circle(double x, double y, double radius)
{
  this->x = x;
  this->y = y;
  this->radius = radius;
}

void
Circle::CircleSetStyle(lineStyle thisCircleStyle)
{
  this->style = thisCircleStyle;
}

lineStyle
Circle::CircleGetStyle(void)
{
  return (this->style);
}

void
Circle::CircleWriteOutput(ofstream &opStream)
{
  string styleString;

  styleString = CircleGetStyleString(*this);
  opStream << "set object circle at " << x << "," << y
           << " size " << radius << " " 
	   << styleString << endl;
}

