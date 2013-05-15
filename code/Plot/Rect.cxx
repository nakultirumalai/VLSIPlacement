# include <Plot.h>

Rect::Rect() 
{
}

Rect::~Rect()
{
}

Rect::Rect(double left, double bot, double right, double top)
{
  this->left = left;
  this->bot = bot;
  this->right = right;
  this->top = top;
  labelPos = RIGHT_TOP;
}

void
Rect::RectSetStyle(lineStyle thisRectStyle)
{
  this->rectStyle = thisRectStyle;
}

void
Rect::RectSetLabel(string rectLabel)
{
  this->label = rectLabel;
}

void
Rect::RectSetLabelPos(labelPosition labelPos)
{
  this->labelPos = labelPos;
}

lineStyle
Rect::RectGetStyle(void)
{
  return (this->rectStyle);
}

void
Rect::RectWriteOutput(ofstream &opStream)
{
  string styleString;

  styleString = RectGetStyleString(*this);
  opStream << "set obj rect from " << left << "," << bot
	   << " to " << right << "," << top
	   << " " << styleString << endl;
  
  //  opStream << "set label at " << left << "," << bot 
  //	   << " \"" << label << "\" " << "front left" 
  //	   << endl;
}

