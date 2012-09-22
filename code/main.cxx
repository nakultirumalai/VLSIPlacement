# include <common.h>
# include <Design.h>

int main(int argc, char *argv[])
{
  string designName, designPath;

  if (argc > 1 && argc < 4) {
    designPath = string(argv[1]);
    designName = string(argv[2]);
    Design myDesign;
    
    myDesign.DesignReadDesign(designPath, designName);
  }

  return 0;
}
