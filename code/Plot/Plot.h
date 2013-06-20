# ifndef PLOT_H
# define PLOT_H

# include <common.h>
# include <Cell.h>
# include <Pin.h>
# include <PhysRow.h>
# include <CellSpread.h>
# include <Design.h>
# include <Legalize.h>

/*******************************************************************************
  FUNCTIONS FOR PLOTTING OBJECTS INTO GNUPLOT
*******************************************************************************/
typedef enum {
  NORMAL_LINE = 0, 
  HIGHLIGHTED_LINE,
  NORMAL_RECT,
  HIGHLIGHTED_RECT,
  PORT_RECT,
  BIN_RECT,
  STRETCHED_BIN_RECT,
  SUPPLY_BIN_RECT,
  DEMAND_BIN_RECT,
  BIN_PATH_RECT,
  REGULAR_PIN,
  HIGHLIGHTED_PIN,
} lineStyle;

typedef enum {
  RIGHT_TOP = 0,
  LEFT_BOTTOM,
} labelPosition;

class Design;
class LegalizeBin;

class Line
{
 private:
  double x1, y1, x2, y2;
  lineStyle style;
 public:
  Line();
  Line(double, double, double, double);
  ~Line();
  
  /* Set functions */
  void LineSetStyle(lineStyle);

  /* Get functions */
  lineStyle LineGetStyle(void);
  
  /* Write output */
  void LineWriteOutput(ofstream &opStream);
};

class Circle
{
 private:
  double x, y, radius;
  lineStyle style;
 public:
  Circle();
  Circle(double, double, double);
  ~Circle();
  
  /* Set functions */
  void CircleSetStyle(lineStyle style);
  
  /* Get functions */
  lineStyle CircleGetStyle(void);
  
  /* Write output */
  void CircleWriteOutput(ofstream &opStream);
};

class Rect
{
 private:
  double left, top, right, bot;
  lineStyle rectStyle;
  labelPosition labelPos;
  string label;

 public:
  Rect();
  Rect(double, double, double, double);
  ~Rect();

  /* Set functions */
  void RectSetStyle(lineStyle);
  void RectSetLabel(string);
  void RectSetLabelPos(labelPosition);

  /* Get functions */
  lineStyle RectGetStyle(void);
  string RectGetLabel(void);
  labelPosition RectGetLabelPos(void);

  /* Write output */
  void RectWriteOutput(ofstream &opStream);
};

class Plot
{
 private:
  string plotTitle;
  string plotFileName;
  double minx, miny, maxx, maxy;
  vector<Line> lines;
  vector<Line> highlightedLines;
  vector<Rect> ports;
  vector<Rect> normalRects;
  vector<Rect> highlightedRects;
  vector<Rect> bins;
  vector<Rect> stretchedBins;
  vector<Rect> supplyBins;
  vector<Rect> demandBins;
  vector<Rect> binPathRects;
  vector<Circle> circles;
  vector<Circle> highlightedCircles;
  ofstream plotOpFile;
  
 public:
  /* Constructors for plotting */
  Plot();
  Plot(string title);
  Plot(string title, string plotFileName);
  Plot(double minx, double miny, double maxx, double maxy);
  ~Plot();
  
  /* Set functions */
  void PlotSetTitle(string title);
  void PlotSetFileName(string plotFileName);
  void PlotSetBoundary(double minx, double miny, double maxx, 
		       double maxy);
  void PlotSetBoundary(Design& myDesign);

  /* Get functions */
  string PlotGetTitle(void);
  string PlotGetFileName(void);
  void PlotGetBoundary(double &minx, double &miny, double &maxx, 
		       double &maxy);
  
  /* Adding Lines */
  void PlotAddLine(double x1, double y1, double x2, double y2);

  /* Adding objects */
  void PlotAddCell(Cell &);
  void PlotAddCells(vector<Cell *> &);
  void PlotHighlightCell(Cell &);
  void PlotAddRow(PhysRow &);
  void PlotAddPort(Cell &);
  void PlotAddBin(Bin &);
  void PlotAddPin(Pin &);
  void PlotAddHighlightedPin(Pin &);
  void PlotAddSupplyBin(LegalizeBin &, int);
  void PlotAddDemandBin(LegalizeBin &, int);
  void PlotAddBinPathRect(LegalizeBin &, int);
  void PlotAddStretchedBin(Bin &);

  /* Write output */
  void PlotWriteOutput(void);
};

string LineGetStyleString(Line &);
string RectGetStyleString(Rect &);
string CircleGetStyleString(Circle &);

# endif

