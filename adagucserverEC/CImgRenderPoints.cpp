/******************************************************************************
 * 
 * Project:  ADAGUC Server
 * Purpose:  ADAGUC OGC Server
 * Author:   Maarten Plieger, plieger "at" knmi.nl
 * Date:     2013-06-01
 *
 ******************************************************************************
 *
 * Copyright 2013, Royal Netherlands Meteorological Institute (KNMI)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 ******************************************************************************/

#include "CImgRenderPoints.h"
#include <set>

const char *CImgRenderPoints::className="CImgRenderPoints";

void CImgRenderPoints::render(CImageWarper*warper, CDataSource*dataSource, CDrawImage*drawImage){
  bool drawVector = false;
  bool drawPoints = true;
  bool drawBarb = false;
  bool drawDiscs = false;
  bool drawText = true;
  bool drawVolume = false;
  
  bool doThinning=false;
  int thinningRadius = 25;
  
  CT::string drawPointPointStyle("point");
  const char *drawPointFontFile = dataSource->srvParams->cfg->WMS[0]->ContourFont[0]->attr.location.c_str();
  int drawPointFontSize=15;
  int drawPointDiscRadius = 8;
  int drawPointTextRadius=drawPointDiscRadius+2;
  bool drawPointDot=true;
  float drawPointAngleStart=-90;
  float drawPointAngleStep=180;
  bool drawPointPlotStationId = false;
  CT::string drawPointTextFormat("%0.1f");
  CColor drawPointTextColor(0,0,0,255);
  CColor drawPointFillColor(0,0,0,128);
  CColor drawPointLineColor(0,0,0,255);
  
  CColor drawBarbLineColor(0,0,128,255);
  float drawBarbLineWidth=1.0;
  bool drawBarbPlotStationId=false;
  CT::string drawBarbBarbStyle("disc");
  
  std::set<std::string> usePoints;
  std::set<std::string> skipPoints;
  bool useFilter=false;
  
//   useFilter=true;
//   usePoints.insert("VKDobs");
//   usePoints.insert("A310a");
//   usePoints.insert("A370");
//   usePoints.insert("A260a");
//   usePoints.insert("A279a");
//   usePoints.insert("A277a");
  
  CDBDebug("style settings: %s", settings.c_str());
  if(settings.indexOf("vector")!=-1){
    drawVector = true;
  }
//   if(settings.indexOf("barb")!=-1){
//     drawBarb = true;
//   }

  CDBDebug("point style before: %d %d %f %f t:#%02x%02x%02x%02x f:#%02x%02x%02x%02x l:#%02x%02x%02x%02x",drawPointDiscRadius, drawPointDot, drawPointAngleStart, drawPointAngleStep, 
            drawPointTextColor.r,drawPointTextColor.g,drawPointTextColor.b,drawPointTextColor.a,
            drawPointFillColor.r,drawPointFillColor.g,drawPointFillColor.b,drawPointFillColor.a,
            drawPointLineColor.r,drawPointLineColor.g,drawPointLineColor.b,drawPointLineColor.a
          );
  
  CStyleConfiguration *styleConfiguration = dataSource->getStyle();
  if(styleConfiguration!=NULL){
    if(styleConfiguration->styleConfig!=NULL){
      CServerConfig::XMLE_Style* s = styleConfiguration->styleConfig;
      if(s -> Point.size() == 1){
        if(s -> Point[0]->attr.fillcolor.empty()==false){
          drawPointFillColor.parse(s -> Point[0]->attr.fillcolor.c_str());
        }
        if(s -> Point[0]->attr.linecolor.empty()==false){
          drawPointLineColor.parse(s -> Point[0]->attr.linecolor.c_str());
        }
        if(s -> Point[0]->attr.textcolor.empty()==false){
          drawPointTextColor.parse(s -> Point[0]->attr.textcolor.c_str());
        }
        if(s -> Point[0]->attr.fontfile.empty()==false){
          drawPointFontFile = s -> Point[0]->attr.fontfile.c_str();
        }
        if(s -> Point[0]->attr.fontsize.empty()==false){
          drawPointFontSize = s -> Point[0]->attr.fontsize.toFloat();
        }
        if(s -> Point[0]->attr.discradius.empty()==false){
          drawPointDiscRadius = s -> Point[0]->attr.discradius.toInt();
          if(s -> Point[0]->attr.textradius.empty()==true){
            drawPointTextRadius = drawPointDiscRadius+4;
          }
        }      
        if(s -> Point[0]->attr.textradius.empty()==false){
          drawPointTextRadius = s -> Point[0]->attr.textradius.toInt();
        } 
        if(s -> Point[0]->attr.dot.empty()==false){
          drawPointDot = s -> Point[0]->attr.dot.equalsIgnoreCase("true");
        }      
        if(s -> Point[0]->attr.anglestart.empty()==false){
          drawPointAngleStart = s -> Point[0]->attr.anglestart.toFloat();
        }      
        if(s -> Point[0]->attr.anglestep.empty()==false){
          drawPointAngleStep = s -> Point[0]->attr.anglestep.toFloat();
        }      
        if(s -> Point[0]->attr.textformat.empty()==false){
          drawPointTextFormat = s -> Point[0]->attr.textformat.c_str();
        }      
        if(s -> Point[0]->attr.pointstyle.empty()==false){
          drawPointPointStyle = s -> Point[0]->attr.pointstyle.c_str();
        }
        if(s -> Point[0]->attr.plotstationid.empty()==false){
          drawPointPlotStationId=s -> Point[0]->attr.plotstationid.equalsIgnoreCase("true");
        }

      }
    }
  }
  CDBDebug("point style after: %d %d %f %f t:#%02x%02x%02x%02x f:#%02x%02x%02x%02x l:#%02x%02x%02x%02x",drawPointDiscRadius, drawPointDot, drawPointAngleStart, drawPointAngleStep, 
            drawPointTextColor.r,drawPointTextColor.g,drawPointTextColor.b,drawPointTextColor.a,
            drawPointFillColor.r,drawPointFillColor.g,drawPointFillColor.b,drawPointFillColor.a,
            drawPointLineColor.r,drawPointLineColor.g,drawPointLineColor.b,drawPointLineColor.a
          );
  
  if (drawPointPointStyle.equalsIgnoreCase("disc")) {
    drawDiscs=true;
    drawPoints=false;
    drawVolume=false;
    drawText=true;
  } else if (drawPointPointStyle.equalsIgnoreCase("volume")){
    drawPoints=false;
    drawVolume=true;
    drawDiscs=false;
    drawText=false;
  } else {
    drawPoints=true;
    drawDiscs=false;
    drawVolume=false;
    drawText=true;
  }
  CDBDebug("drawPoints=%d drawText=%d drawBarb=%d drawVector=%d drawVolume=%d", drawPoints, drawText, drawBarb, drawVector, drawVolume);
  
  // CStyleConfiguration *styleConfiguration = dataSource->getStyle();
  if(styleConfiguration!=NULL){
    if(styleConfiguration->styleConfig!=NULL){
      CServerConfig::XMLE_Style* s = styleConfiguration->styleConfig;
      if(s -> FilterPoints.size() == 1){
        if(s -> FilterPoints[0]->attr.use.empty()==false){
          CT::string filterPointsUse=s -> FilterPoints[0]->attr.use.c_str();
          std::vector<CT::string> use=filterPointsUse.splitToStack(",");
          for (std::vector<CT::string>::iterator it=use.begin(); it != use.end(); ++it) {
            CDBDebug("adding %s to usePoints", it->c_str());
            usePoints.insert(it->c_str());
          }
          useFilter=true;
        }
        if(s -> FilterPoints[0]->attr.skip.empty()==false){
          CT::string filterPointsSkip=s -> FilterPoints[0]->attr.skip.c_str();
          useFilter=true;
          std::vector<CT::string> skip=filterPointsSkip.splitToStack(",");
          for (std::vector<CT::string>::iterator it=skip.begin(); it != skip.end(); ++it) {
            skipPoints.insert(it->c_str());
          }
        }
      }
    }
  }
  
  if(settings.indexOf("thin")!=-1){
    doThinning = true;
    if(styleConfiguration!=NULL){
      if(styleConfiguration->styleConfig!=NULL){
        CServerConfig::XMLE_Style* s = styleConfiguration->styleConfig;
        if(s -> Thinning.size() == 1){
          if(s -> Thinning[0]->attr.radius.empty()==false){
            thinningRadius = s -> Thinning[0]->attr.radius.toInt();
            //CDBDebug("Thinning radius = %d",s -> Thinning[0]->attr.radius.toInt());
          }
        }
      }
    }
  }
  
  int alphaPoint[(2*drawPointDiscRadius+1)*(2*drawPointDiscRadius+1)];
  if (drawVolume) {
    int p = 0;
    for(int y1=-drawPointDiscRadius;y1<=drawPointDiscRadius;y1++){
      for(int x1=-drawPointDiscRadius;x1<=drawPointDiscRadius;x1++){
        float d= sqrt(x1*x1+y1*y1); //between 0 and 1.4*drawPointDiscRadius
        d=drawPointDiscRadius-d;
//        d=10-d;
        if (d<0)d=0;
        d=d*2.4;
        alphaPoint[p++] = d;
      }
    }
  }
  
  if(dataSource->getNumDataObjects()!=2){ // Not for vector (u/v or speed/dir pairs) TODO
    for (size_t dataObject=0; dataObject<dataSource->getNumDataObjects(); dataObject++) {
      std::vector<PointDVWithLatLon> *pts=&dataSource->getDataObject(dataObject)->points;
      float useangle=drawPointAngleStart+drawPointAngleStep*dataObject;
      int kwadrant=0;
      if (useangle<0) {
        kwadrant=3-int(useangle/90);
      } else {
        kwadrant=int(useangle/90);
      }
      float usedx=drawPointTextRadius*sin(useangle*M_PI/180);
      float usedy=drawPointTextRadius*cos(useangle*M_PI/180);
//      CDBDebug("angles[%d] %f %d %f %f", dataObject, useangle, kwadrant, usedx, usedy);
    
      //THINNING
      std::vector<PointDVWithLatLon> *p1=&dataSource->getDataObject(dataObject)->points;
      size_t l=p1->size();
      size_t nrThinnedPoints=l;
      std::vector<size_t> thinnedPointsIndex;
      
      if (doThinning) {
        for(size_t j=0;j<l;j++){
          size_t nrThinnedPoints=thinnedPointsIndex.size();
          size_t i;
          if ((useFilter && (*p1)[j].paramList.size()>0 && 
            usePoints.find((*p1)[j].paramList[0].value.c_str())!=usePoints.end())||
               !useFilter) {
            for(i=0;i<nrThinnedPoints;i++){ 
              if (j==0)break; //Always put in first element
              if(hypot((*p1)[thinnedPointsIndex[i]].x-(*p1)[j].x,(*p1)[thinnedPointsIndex[i]].y-(*p1)[j].y)<thinningRadius)break;
            }
            if(i==nrThinnedPoints)thinnedPointsIndex.push_back(j);      
          }
        }
        nrThinnedPoints=thinnedPointsIndex.size();
      } else if (useFilter) {
        for(size_t j=0;j<l;j++){
          if ((*p1)[j].paramList.size()>0 && 
            usePoints.find((*p1)[j].paramList[0].value.c_str())!=usePoints.end()) {
            //if ((*p1)[j].paramList.size()>0 && usePoints.find((*p1)[j].paramList[0].value.c_str())!=usePoints.end()){
            thinnedPointsIndex.push_back(j);      
            CDBDebug("pushed el %d: %s", j, (*p1)[j].paramList[0].value.c_str());
          }
        }
        nrThinnedPoints=thinnedPointsIndex.size();
      } else {
        //if no thinning: get all indexes
        for (size_t pointIndex=0; pointIndex<l; pointIndex++){
          thinnedPointsIndex.push_back(pointIndex);
        }
        nrThinnedPoints=thinnedPointsIndex.size(); 
      }
      
      CT::string t;
      for(size_t pointNo=0;pointNo<nrThinnedPoints;pointNo++){
        size_t j=pointNo;
        j=thinnedPointsIndex[pointNo];

        if(drawVolume){
          int x=(*pts)[j].x;
          int y=dataSource->dHeight-(*pts)[j].y;
          int rvol=drawPointFillColor.r;
          int gvol=drawPointFillColor.g;
          int bvol=drawPointFillColor.b;
          
          drawImage->setPixelTrueColor(x,y, 0,0,0,255);
          int *p=alphaPoint;
          for(int y1=-drawPointDiscRadius;y1<=drawPointDiscRadius;y1++){
            for(int x1=-drawPointDiscRadius;x1<=drawPointDiscRadius;x1++){
              drawImage->setPixelTrueColor(x+x1,y+y1, rvol, gvol, bvol, *p++);
            }
          }
          if (drawPointPlotStationId) {
          }
        }
        
        if(drawPoints){
          int x=(*pts)[j].x;
          int y=dataSource->dHeight-(*pts)[j].y;
          if (drawPointDot) drawImage->circle(x,y, 1, drawPointFillColor,0.65);
          float v=(*pts)[j].v;
          if(v==v){
            t.print(drawPointTextFormat.c_str(),v);
//              CDBDebug("[%d] v=%f: drawPointDiscRadius=%d n=%d index=%d", j, v, drawPointDiscRadius, dataSource->getNumDataObjects(), getPixelIndexForValue(dataSource, v));
            if (drawPointDiscRadius==0) {
//                CDBDebug("radius==0 => centeredtext only %f", v);
              if (drawPointPlotStationId) {
                drawImage->drawCenteredText(x,y+drawPointTextRadius+3, drawPointFontFile, drawPointFontSize, 0, t.c_str(), drawPointTextColor);
              } else {
                drawImage->drawCenteredText(x,y, drawPointFontFile, drawPointFontSize, 0, t.c_str(), drawPointTextColor);
              }
            } else {
              if (dataSource->getNumDataObjects()==1) {
                int pointColorIndex=getPixelIndexForValue(dataSource, v);
                drawImage->setDisc(x, y, drawPointDiscRadius, pointColorIndex, pointColorIndex);
                drawImage->drawCenteredText(x,y+drawPointTextRadius, drawPointFontFile, drawPointFontSize, 0, t.c_str(), drawPointTextColor);
              } else {
                drawImage->setDisc(x, y, drawPointDiscRadius, drawPointFillColor, drawPointLineColor);
                drawImage->drawAnchoredText(x+usedx,y-usedy, drawPointFontFile, drawPointFontSize, 0, t.c_str(), drawPointTextColor, kwadrant);
              }
            }
            if (drawPointPlotStationId) {
              if((*pts)[j].paramList.size()>0){
                CT::string value =(*pts)[j].paramList[0].value;
                drawImage->drawCenteredText(x,y-drawPointTextRadius-3, drawPointFontFile, drawPointFontSize, 0, value.c_str(), drawPointTextColor);
              }
            }
          }else{
            CDBDebug("Value not available");
            if((*pts)[j].paramList.size()>0){
              CT::string value = (*pts)[j].paramList[0].value;
              CDBDebug("Extra value: %s fixed color with radius %d", value.c_str(), drawPointDiscRadius);
              if (drawPointDiscRadius==0) {
                drawImage->drawCenteredText(x,y, drawPointFontFile, drawPointFontSize, 0, value.c_str(), drawPointTextColor);                  
              } else {
                drawImage->drawAnchoredText(x+usedx,y-usedy, drawPointFontFile, drawPointFontSize, 0, value.c_str(), drawPointTextColor, kwadrant);
              }
            }
          }
        }
        if (drawDiscs) { //Filled disc with circle around it and value inside
          int x=(*pts)[j].x;
          int y=dataSource->dHeight-(*pts)[j].y;
          //drawImage->circle(x,y, drawPointDiscRadius, 240,0.65);
          float v=(*pts)[j].v;
          if(v==v){
            t.print(drawPointTextFormat.c_str(),v);
            drawImage->setTextDisc( x, y,drawPointDiscRadius, t.c_str(),drawPointFontFile, drawPointFontSize,drawPointTextColor,drawPointFillColor, drawPointLineColor);
          }
        }
      }
    }
  }
  
  if(dataSource->getNumDataObjects()==2){
    CDBDebug("VECTOR");
    CStyleConfiguration *styleConfiguration = dataSource->getStyle();
    CDBDebug("styleConfiguration=%x", styleConfiguration);
    if(styleConfiguration!=NULL){
      CDBDebug("styleConfiguration->styleConfig=%x", styleConfiguration->styleConfig);
      if(styleConfiguration->styleConfig!=NULL){
        CServerConfig::XMLE_Style* s = styleConfiguration->styleConfig;
        CDBDebug("XMLE_Style: %x", s);
        if(s -> Barb.size() == 1){
          CDBDebug("size=%d", s -> Barb.size());
          if(s -> Barb[0]->attr.linecolor.empty()==false){
            drawBarbLineColor.parse(s -> Barb[0]->attr.linecolor.c_str());
          }
          if(s -> Barb[0]->attr.barbstyle.empty()==false){
            drawBarbBarbStyle=s -> Barb[0]->attr.barbstyle.c_str();
            if (drawBarbBarbStyle.equalsIgnoreCase("barb")) { drawBarb=true; }
            else if (drawBarbBarbStyle.equalsIgnoreCase("disc")) { drawDiscs=true;}
            else if (drawBarbBarbStyle.equalsIgnoreCase("vector")) { drawVector=true; }
            else {drawBarb=true;}
          }

          if(s -> Barb[0]->attr.linewidth.empty()==false){
            drawBarbLineWidth=s -> Barb[0]->attr.linewidth.toFloat();
          }
          if(s -> Barb[0]->attr.plotstationid.empty()==false){
            drawBarbPlotStationId=s -> Barb[0]->attr.plotstationid.equalsIgnoreCase("true");
          }             
        }
      }
    }
    CDBDebug("Vectors drawBarb=%d drawDiscs=%d drawVector=%d", drawBarb, drawDiscs, drawVector);

    CT::string varName1=dataSource->getDataObject(0)->cdfVariable->name.c_str();
    CT::string varName2=dataSource->getDataObject(1)->cdfVariable->name.c_str();
    std::vector<PointDVWithLatLon> *p1=&dataSource->getDataObject(0)->points;
    std::vector<PointDVWithLatLon> *p2=&dataSource->getDataObject(1)->points;
    size_t l=p1->size();
    size_t nrThinnedPoints=l;
    std::vector<size_t> thinnedPointsIndex;    
    
    CT::string t;
    if (doThinning) {
      for(size_t j=0;j<l;j++){
        size_t nrThinnedPoints=thinnedPointsIndex.size();
        size_t i;
        if ((useFilter && (*p1)[j].paramList.size()>0 && 
          usePoints.find((*p1)[j].paramList[0].value.c_str())!=usePoints.end())||
              !useFilter) {
          for(i=0;i<nrThinnedPoints;i++){ 
            if (j==0)break; //Always put in first element
            if(hypot((*p1)[thinnedPointsIndex[i]].x-(*p1)[j].x,(*p1)[thinnedPointsIndex[i]].y-(*p1)[j].y)<thinningRadius)break;
          }
          if(i==nrThinnedPoints)thinnedPointsIndex.push_back(j);      
        }
      }
      nrThinnedPoints=thinnedPointsIndex.size();
    } else if (useFilter) {
      for(size_t j=0;j<l;j++){
        if ((*p1)[j].paramList.size()>0 && 
          usePoints.find((*p1)[j].paramList[0].value.c_str())!=usePoints.end()) {
          //if ((*p1)[j].paramList.size()>0 && usePoints.find((*p1)[j].paramList[0].value.c_str())!=usePoints.end()){
          thinnedPointsIndex.push_back(j);      
          CDBDebug("pushed el %d: %s", j, (*p1)[j].paramList[0].value.c_str());
        }
      }
      nrThinnedPoints=thinnedPointsIndex.size();
    } else {
      //if no thinning: get all indexes
      for (size_t pointIndex=0; pointIndex<l; pointIndex++){
        thinnedPointsIndex.push_back(pointIndex);
      }
      nrThinnedPoints=thinnedPointsIndex.size(); 
    }
    CDBDebug("Vector plotting %d elements %d %d", nrThinnedPoints, useFilter, usePoints.size());
      
    for(size_t pointNo=0;pointNo<nrThinnedPoints;pointNo++){

      size_t j=pointNo;
      j=thinnedPointsIndex[pointNo];
      int x=(*p1)[j].x;
      double lat=(*p1)[j].lat;
      double lon=(*p1)[j].lon;
      double rotation=(*p1)[j].rotation;
      
      if(rotation == 0){
        double latForRot=lat;
        double lonForRot=lon;
        double latOffSetForRot=lat-0.01;
        double lonOffSetForRot=lon;
        bool projectionRequired = true;
  
          
        if(projectionRequired){
          warper->reprojfromLatLon(lonForRot,latForRot);
          warper->reprojfromLatLon(lonOffSetForRot,latOffSetForRot);
        }
       
        if(projectionRequired){
          double dy=latForRot- latOffSetForRot;
          double dx=lonForRot - lonOffSetForRot;
          rotation= -(atan2(dy,dx)/(3.141592654))*180+90;
        }
      }
      
      int y=dataSource->dHeight-(*p1)[j].y;
      
      float strength = (*p1)[j].v;
      float direction = (*p2)[j].v+rotation;//direction=rotation;
     // direction=360-45;
      //drawImage->drawVector(x, y, ((90-direction)/360)*3.141592654*2, strength*2, 240);
      if (drawBarb || drawVector||drawDiscs) {

        if(drawBarb){
          if(lat>0){
            drawImage->drawBarb(x, y, ((270-direction)/360)*3.141592654*2, strength, drawBarbLineColor ,drawBarbLineWidth, false, false);
          }else{
            drawImage->drawBarb(x, y, ((270-direction)/360)*3.141592654*2, strength, drawBarbLineColor, drawBarbLineWidth, true, true);
          }
        }
        if(drawVector){
          drawImage->drawVector(x, y, ((270-direction)/360)*3.141592654*2, strength*2, drawBarbLineColor, drawBarbLineWidth);
        }
        //void drawBarb(int x,int y,double direction, double strength,int color,bool toKnots,bool flip);
        if(drawBarbPlotStationId){
          if((*p1)[j].paramList.size()>0){
              CT::string value = (*p1)[j].paramList[0].value;
              drawImage->setText(value.c_str(), value.length(),x-value.length()*3,y-20, drawPointTextColor, 0);
            }
        }
        if (drawDiscs) {
          // Draw a disc with the speed value in text and the dir. value as an arrow
          int x=(*p1)[j].x;
          int y=dataSource->dHeight-(*p1)[j].y;
          float dir = (*p2)[j].v;
          if (dir==dir) {
            drawImage->drawVector(x, y, ((270-direction)/360)*3.141592654*2, drawPointDiscRadius+12, drawPointFillColor, drawBarbLineWidth*4);
          }
          float v=(*p1)[j].v;
          if(v==v){
            t.print(drawPointTextFormat.c_str(),v);
            drawImage->setTextDisc( x, y, drawPointDiscRadius, t.c_str(),drawPointFontFile, drawPointFontSize,drawPointTextColor,drawPointFillColor, drawPointLineColor);
          }
        }
      }
    }
  }
}
int CImgRenderPoints::set(const char*values){

  settings.copy(values);
  return 0;
}

int CImgRenderPoints::getPixelIndexForValue(CDataSource*dataSource,float val){
      bool isNodata=false;
      
      if(dataSource->getDataObject(0)->hasNodataValue){
        if(val==float(dataSource->getDataObject(0)->dfNodataValue))isNodata=true;
        if(!(val==val))isNodata=true;
      }
      if(!isNodata) {
        CStyleConfiguration *styleConfiguration = dataSource->getStyle();
        if(styleConfiguration->hasLegendValueRange==1)
          if(val<styleConfiguration->legendLowerRange||val>styleConfiguration->legendUpperRange)isNodata=true;
          if(!isNodata){
            if(styleConfiguration->legendLog!=0)val=log10(val+.000001)/log10(styleConfiguration->legendLog);
            val*=styleConfiguration->legendScale;
            val+=styleConfiguration->legendOffset;
            if(val>=239)val=239;else if(val<0)val=0;
            return int(val);
          }
      }
      return 0; 
}
