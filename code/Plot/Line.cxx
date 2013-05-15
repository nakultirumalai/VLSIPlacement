# include <Plot.h>

Line::Line()
{
}

Line::~Line()
{
}

Line::Line(double x1, double y1, double x2, double y2)
{
  this->x1 = x1;
  this->y1 = y1;
  this->x2 = x2;
  this->y2 = y2;
}

void
Line::LineSetStyle(lineStyle thisLineStyle)
{
  this->style = thisLineStyle;
}

lineStyle
Line::LineGetStyle(void)
{
  return (this->style);
}

void
Line::LineWriteOutput(ofstream &opStream)
{
  string styleString;

  styleString = LineGetStyleString(*this);
  opStream << "set arrow from " << x1 << "," << y1
           << " to " << x2 << "," << y2
           << " " << styleString << "  nohead" << endl;
}

