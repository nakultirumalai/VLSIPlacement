# include <common.h>
# include <stdlib.h>

void 
common_error(std::string ErrorString)
{
# ifdef COMMON_DEBUG  
  std::cout << std::endl << "Error: " << ErrorString << std::endl << "Quitting.." << std::endl;
  exit(0);
# endif
}

void 
common_message(std::string MessageString)
{
# ifdef COMMON_DEBUG
  //  std::cout << std::endl << "Message from program:" << MessageString << std::endl;
# endif 
}


string 
getStrFromInt(int thisInt)
{
  stringstream oss;
  
  oss << thisInt;
  
  return oss.str();
}

/* Rounds a double/float to a single decimal point */
double dround(double thisDouble)
{
  long int thisInt;
  double absDouble;

  /* Get the integer part of thisDouble */
  thisInt = (long int)trunc(thisDouble);
  thisDouble = thisDouble - thisInt;
  
  absDouble = fabs(thisDouble);
  if (absDouble > 0 && absDouble <= 0.05) thisDouble = (double)thisInt;
  else if (absDouble > 0.05 && absDouble <= 0.1) (thisDouble < 0)?(thisDouble = (double)thisInt-0.1):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.1 && absDouble <= 0.15) (thisDouble < 0)?(thisDouble = (double)thisInt-0.1):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.15 && absDouble <= 0.2) (thisDouble < 0)?(thisDouble = (double)thisInt-0.2):(thisDouble = (double)thisInt+0.2);
  else if (absDouble > 0.2 && absDouble <= 0.25) (thisDouble < 0)?(thisDouble = (double)thisInt-0.2):(thisDouble = (double)thisInt+0.2);
  else if (absDouble > 0.25 && absDouble <= 0.3) (thisDouble < 0)?(thisDouble = (double)thisInt-0.3):(thisDouble = (double)thisInt+0.3);
  else if (absDouble > 0.3 && absDouble <= 0.35) (thisDouble < 0)?(thisDouble = (double)thisInt-0.3):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.35 && absDouble <= 0.4) (thisDouble < 0)?(thisDouble = (double)thisInt-0.4):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.4 && absDouble <= 0.45) (thisDouble < 0)?(thisDouble = (double)thisInt-0.4):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.45 && absDouble <= 0.5) (thisDouble < 0)?(thisDouble = (double)thisInt-0.5):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.5 && absDouble <= 0.55) (thisDouble < 0)?(thisDouble = (double)thisInt-0.5):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.55 && absDouble <= 0.6) (thisDouble < 0)?(thisDouble = (double)thisInt-0.6):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.6 && absDouble <= 0.65) (thisDouble < 0)?(thisDouble = (double)thisInt-0.6):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.65 && absDouble <= 0.7) (thisDouble < 0)?(thisDouble = (double)thisInt-0.7):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.7 && absDouble <= 0.75) (thisDouble < 0)?(thisDouble = (double)thisInt-0.7):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.75 && absDouble <= 0.8) (thisDouble < 0)?(thisDouble = (double)thisInt-0.8):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.8 && absDouble <= 0.85) (thisDouble < 0)?(thisDouble = (double)thisInt-0.8):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.85 && absDouble <= 0.9) (thisDouble < 0)?(thisDouble = (double)thisInt-0.9):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.9 && absDouble <= 0.95) (thisDouble < 0)?(thisDouble = (double)thisInt-0.9):(thisDouble = (double)thisInt+0.1);
  else if (absDouble > 0.95 && absDouble <= 1.0) (thisDouble < 0)?(thisDouble = (double)thisInt):(thisDouble = (double)thisInt);
  
  return (thisDouble);
}
