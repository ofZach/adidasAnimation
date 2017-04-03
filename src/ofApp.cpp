#include "ofApp.h"


ofParameter < bool > useOrtho;
ofParameter < float > sliceZpush;
ofParameter < float > zOffset;

ofParameter < float > traceSpeedLarge;
ofParameter < float > traceSpeedSlow;



ofParameter < int > blurImageAmount;
ofParameter < int > numLayers;
ofParameter < float > layerMin;
ofParameter < float > layerMax;
ofParameter < int > polylineResampleSize;
ofParameter < int > polylineSmoothAmount;
ofParameter < float > layerPower;

ofParameter < bool > makeNewLayers;



//--------------------------------------------------------------
void ofApp::setup(){

    group.setName("settings");
    group.add(useOrtho.set("useOrtho", true));
    group.add(sliceZpush.set("sliceZpush", 5, 0,100));
    group.add(zOffset.set("zOffset", 0, -800,800));
    
    group.add(traceSpeedLarge.set("traceSpeedLarge", 6, 0,50));
    group.add(traceSpeedSlow.set("traceSpeedSlow", 3, 0,50));
    
    
    group.add(blurImageAmount.set("blurImageAmount", 21, 1,50));
    group.add(numLayers.set("numLayers", 40, 5,70));
    group.add(layerMin.set("layerMin", 0, 0,255));
    group.add(layerMax.set("layerMax", 255, 0,255));
    
    group.add(polylineSmoothAmount.set("polylineSmoothAmount", 5, 1, 33));
    group.add(polylineResampleSize.set("polylineResampleSize", 5, 1,20));
    group.add(layerPower.set("layerPower", 1.3, 0.1,8.0));
    group.add(makeNewLayers.set("makeNewLayers", false));
    
    panel.setup();
    panel.add(group);
    panel.loadFromFile("settings.xml");
    

    
    ofDirectory dir;
    dir.listDir("2017-03-31-16-41-36-176");
    
    
    gradientShader.load("shader/gradient.vert", "shader/gradient.frag");
    
    spot.load("spot.png");
    img.load("big.png");
    svg.load("big.svg");
    
    setupLayers();
    
    myTime = 0;
}

void ofApp::setupLayers(){

    lines.clear();
    meshes.clear();
    
    
    minPerim  =100000000;
    maxPerim = 0;
    
    img2.load("bigDepth.png");
    ofxCv::blur(img2,blurImageAmount);
    
    for (int i = 0; i < numLayers; i++){
        float pctTemp =ofMap(i, 0,numLayers-1, 0, 1);
        finder.setThreshold(ofMap(powf(pctTemp, layerPower), 0, 1, layerMin, layerMax));
        finder.setMinArea(100);
        finder.setMaxArea(img.getWidth() * img.getHeight()*0.99);
        finder.findContours(img2);
        
        vector < ofPolyline  > linesTemp;
        for (int j = 0; j < finder.size(); j++){
            linesTemp.push_back(finder.getPolyline(j).getResampledBySpacing(polylineResampleSize).getSmoothed(polylineSmoothAmount));
            
        }
        
        
            for (auto & line : linesTemp ){
                for (auto & ptrt : line ){
                    ptrt = ptrt - ofPoint(img.getWidth()/2, img.getHeight()/2);
                }
                float perim = line.getPerimeter();
                minPerim = MIN(minPerim, perim);
                maxPerim = MAX(maxPerim, perim);
                

            }
        
        lines.push_back(linesTemp);
        

    }
   
    //cout << minPerim << " " << " " << maxPerim << endl;
    

    
//    for (int i = 0; i < dir.size(); i++){
//        
//        ofImage temp;
//        temp.load(dir.getPath(i));
//        float scale = img.getWidth() / temp.getWidth();
//        //finder.setThreshold(ofMap(i, 0, 40, 1, 255));
//        //finder.setMinArea(100);
//        //finder.setMaxArea(temp.getWidth() * temp.getHeight()*0.90);
//        finder.findContours(temp);
//        
//        vector < ofPolyline  > linesTemp;
//        for (int j = 0; j < finder.size(); j++){
//            linesTemp.push_back(finder.getPolyline(j).getResampledBySpacing(1).getSmoothed(3));
//        }
//        
//        for (auto & lines : linesTemp){
//           for (auto & pt : lines){
//               pt = pt * scale;
//           }
//        }
//
//        
//        lines.push_back(linesTemp);
//        
//    }
    
    
    
    
    
    for (auto & lineSet : lines){
        
        
        vector < ofMesh > meshSet;
        
        for (auto & line : lineSet){
            
            
                ofPath myShape;
                bool bFirst = true;
                for (auto p : line){
                    ofPoint aaa = p;
                    if (bFirst == true){
                        myShape.moveTo(aaa);
                        bFirst = false;
                    } else {
                        myShape.lineTo(aaa);
                    }
                    //ofVertex(aaa);
                }
                myShape.setColor(0);
                ofPoint aaa = line[0];
                myShape.lineTo(aaa);
                ofMesh m = myShape.getTessellation();
            
            
                for (int z = 0; z < m.getVertices().size(); z++){
                    m.addTexCoord(m.getVertices()[z] + ofPoint(img.getWidth()/2, img.getHeight()/2));
                }
            
            
            
            
                meshSet.push_back(m);
            
        
            
        }
        
        meshes.push_back(meshSet);
        
        
    
    
    
    }
    
    
    ofPolyline bounds = svg.getPathAt(0).getOutline()[0];
    ofRectangle boundsBounds = bounds.getBoundingBox();
    ofRectangle otherBounds = ofRectangle(0,0, img.getWidth(), img.getHeight());
    
    
//    for (int i = 1; i < svg.getNumPath(); i++){
//        ofPolyline me = svg.getPathAt(i).getOutline()[0];
//        for (auto & pt : me){
//            float x = ofMap(pt.x, boundsBounds.getLeft(),
//                               boundsBounds.getRight(),
//                            otherBounds.getLeft(),
//                            otherBounds.getRight());
//            float y = ofMap(pt.y, boundsBounds.getTop(),
//                            boundsBounds.getBottom(),
//                            otherBounds.getTop(),
//                            otherBounds.getBottom());
//            pt.set(x,y);
//            pt -= ofPoint(img.getWidth()/2, img.getHeight()/2);
//        }
//        svgLines.push_back(me);
//    }
//    
    
    
    
}

//--------------------------------------------------------------
void ofApp::update(){

    
    if (makeNewLayers){
        setupLayers();
        makeNewLayers = false;
    }
    
    myTime += ofGetLastFrameTime();
    
    if (ofGetFrameNum() % 60 == 0){
        gradientShader.load("shader/gradient.vert", "shader/gradient.frag");
    }
}


ofEasyCam cam;


float smoothstep(float edge0, float edge1, float x)
{
    // Scale, bias and saturate x to 0..1 range
    x = ofClamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    // Evaluate polynomial
    return x*x*(3 - 2 * x);
}


float getTimeAsAngle(float timeSecs){
    return ofMap(timeSecs, 0, 15, 0, TWO_PI);
}

float calcAngleRamp(float angleToMatch, float currentAngle, float range){
    float diff = angleToMatch - currentAngle;
    while (diff < -PI) diff += TWO_PI;
    while (diff > PI) diff -= TWO_PI;
    diff = fabs(diff);
   // cout << diff << endl;
    
    return ofMap(diff, 0, range, 1, 0, true);
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);

    
    
    float timeMod = fmod(myTime, 15.0);
    float timePct = timeMod / 15.0;
    float timeAsAngle = ofMap(timePct, 0, 1, 0, TWO_PI);
    
    
    //cout << ofGetFrameRate() << endl;
    
    
    
    vector < vector < ofMesh > > meshesProjected;
    meshesProjected = meshes;
    vector < vector < ofPolyline > > linesProjected;
    linesProjected = lines;
    
    
    ofRectangle viewport = ofGetCurrentViewport();
    
    
    
    
    
    cam.begin();
    if (useOrtho) cam.enableOrtho();
    else cam.disableOrtho();
    
    
    float blackLevel =  sin(   smoothstep(0,1,calcAngleRamp(0, timeAsAngle - 0*0.002, getTimeAsAngle(5))*PI/2));

    //cout << calcAngleRamp(0, timeAsAngle, PI/4) << endl;
    
    
    ofMatrix4x4 mat = cam.getModelViewProjectionMatrix(viewport);
    
    //-------------------------------------- project meshes
    int levelCount = 0;
    for (auto & meshSet : meshesProjected){
        
        ofMatrix4x4 matRot;
        
        matRot.rotate(sin(ofGetElapsedTimef()*0.5) * 10, 0,1,0);
        matRot.rotate(-sin(   smoothstep(0,1,calcAngleRamp(0, timeAsAngle - levelCount*0.002, getTimeAsAngle(5))*PI/2))*mouseX, 1,0,0);
        
        
        
        for (auto & meshes : meshSet){
            for (auto & vertex : meshes.getVertices()){
             
                
                
                ofVec3f CameraXYZ = (vertex + ofPoint(0,0,levelCount*sliceZpush + zOffset))*ofPoint(0.5, -0.5, 0.5) *matRot* mat;
                ofVec3f ScreenXYZ;
                
                ScreenXYZ.x = (CameraXYZ.x + 1.0f) / 2.0f * viewport.width + viewport.x;
                ScreenXYZ.y = (1.0f - CameraXYZ.y) / 2.0f * viewport.height + viewport.y;
                ScreenXYZ.z = levelCount*0.1;
                
                vertex = ScreenXYZ;
            }
        }
        levelCount++;
    }
    //-------------------------------------- project lines
    levelCount = 0;
    for (auto & lineSet : linesProjected){
        
        ofMatrix4x4 matRot;
        
        
        matRot.rotate(sin(ofGetElapsedTimef()*0.5) * 10, 0,1,0);
        matRot.rotate(-sin(   smoothstep(0,1,calcAngleRamp(0, timeAsAngle - levelCount*0.002, getTimeAsAngle(5))*PI/2))*mouseX, 1,0,0);
        
        
        for (auto & lines : lineSet){
            for (auto & vertex : lines.getVertices()){
            
                ofVec3f CameraXYZ = (vertex + ofPoint(0,0,levelCount*sliceZpush + zOffset))*ofPoint(0.5, -0.5, 0.5) * matRot*mat;
                ofVec3f ScreenXYZ;
                
                ScreenXYZ.x = (CameraXYZ.x + 1.0f) / 2.0f * viewport.width + viewport.x;
                ScreenXYZ.y = (1.0f - CameraXYZ.y) / 2.0f * viewport.height + viewport.y;
                ScreenXYZ.z = levelCount*0.1+0.0001;
                
                vertex = ScreenXYZ;
            }
        }
        levelCount++;
    }
    
    cam.end();
    
    
    //ofEnableDepthTest();
    
    
    
   //img.bind();
   //gradientShader.setUniformTexture("img", img, 0);
    
//    for (auto & meshSet : meshesProjected){
//        for (auto & meshes : meshSet){
//            ofSetColor(ofRandom(255));
//            meshes.draw();
//        }
//    }
//    
//    for (auto & lineSet : linesProjected){
//        for (auto & lines : lineSet){
//            ofSetColor(255);
//            lines.draw();
//        }
//    }


//    ofSetRectMode(OF_RECTMODE_CENTER);
//    
//    for (int i = 0; i < meshesProjected.size(); i++){
//
//        for (int j = 0; j < meshesProjected[i].size(); j++){
//            auto & mesh = meshesProjected[i][j];
//            auto & line = linesProjected[i][j];
//            
//            
//            ofSetColor(0,0,0,5);
//            ofPushMatrix();
//            ofTranslate(0,0,0);
//            for (int k = 0; k < line.size(); k+=4){
//                //ofCircle(line[k].x, line[k].y, 10);
//                spot.draw(line[k].x, line[k].y, mouseX*0.1, mouseX*0.1);
//            }
//            ofPopMatrix();
//            
//            ofSetColor(255);
//            mesh.draw();
//            
//        }
//    }
    
    
//    //cout << svgLines.size() << endl;
//    
//    ofPoint ptsForLights[18];
//    
//    ofDisableDepthTest();
//    cam.begin();
//    ofPushMatrix();
//    ofScale(0.5, -0.5, 1);
//    ofSetColor(255,0,0);
//    for (int i= 0; i < svgLines.size(); i++){
//        
//        float len = svgLines[i].getPerimeter();
//        float pct = fmod(ofGetElapsedTimef()*500.0, len)/(len);
//        
//        ofPoint pt = svgLines[i].getPointAtPercent(pct);
//        //ofCircle(pt, 50);
//        
//        ptsForLights[i].set( cam.worldToScreen(pt* ofPoint(0.5, -0.5, 1)));
//    }
//    //    for (int i= 0; i < svgLines.size(); i++){
//    //        svgLines[i].draw();
//    //    }
//    ofSetColor(255);
//    ofPopMatrix();
//    //svg.draw();
//    cam.end();

    
    
    
    
    
    gradientShader.begin();
    gradientShader.setUniform1f("windowHeight", ofGetHeight());

//    for (int i = 0; i < 18; i++){
//        ptsForLights[i].y = ofGetHeight() - ptsForLights[i].y;
//    }
//    
//    //ptsForLights[0].set(mouseX, ofGetHeight()-mouseY);
//    
//    
//    GLuint program = gradientShader.getProgram();
//    GLint loc = glGetUniformLocation(program, "lights");
//    glUniform3fv(loc, 18, ptsForLights[0].getPtr());
//    

    img.bind();
    
    gradientShader.setUniformTexture("img", img, 0);
    
    blackLevel = ofMap(smoothstep(0,1,calcAngleRamp(6.0, timeAsAngle, getTimeAsAngle(8))), 0, 1, 1, 0);
    
    gradientShader.setUniform1f("blackLevel", blackLevel);
    
    
    
    //cout << "hi " << endl;
    //cout << ofMap(smoothstep(0,1,calcAngleRamp(7.5, timeAsAngle, getTimeAsAngle(5))), 0, 1, 1, 0) << endl;
    
    
    //cout << blackLevel << " " << endl;
    
    
  float ff = ofGetElapsedTimef();

    
    float lineAmt = ofMap(smoothstep(0,1,calcAngleRamp(0, timeAsAngle, getTimeAsAngle(5))), 0, 1, 0.05, 1.0);

    for (int i = 0; i < meshesProjected.size(); i++){

        for (int j = 0; j < meshesProjected[i].size(); j++){
            auto & mesh = meshesProjected[i][j];
            auto & line = linesProjected[i][j];
            auto & lineOrig = lines[i][j];
            
            float len = lineOrig.getPerimeter();
            
            float speed = ofMap(len, minPerim, maxPerim, traceSpeedSlow, traceSpeedLarge);
            //float pct = fmod(ff*100.0, len*1.1) / len*1.1;
            //ofPoint p = line.getPointAtPercent(pct);

            //p += ofPoint(i, j);
            //line.getClosestPoint(ofPoint(mouseX, mouseY));
            
            //p.y = ofGetHeight()-p.y;
            
            
            float pctToDo = lineAmt;
            pctToDo = MIN(pctToDo, 1.0);
            float pct = fmod(ff + j, speed) / speed;
            float startPct = pct;
            float endPct = fmod(pct + pctToDo, 1.0);
            ofPolyline tempLine;
            
            int mm = line.getIndexAtPercent(startPct);
            int mm2 = line.getIndexAtPercent(endPct);
            
            //cout << "pre " << pct << " " << mm << " " << mm2 << " " << line.size() <<  endl;
            
            
            if (mm2  == mm && pctToDo > 0.1){
                mm2 += line.size();
            }
            
            if (mm2 < mm){
                mm2 += line.size();
            }
            
            //cout << pct << " " << mm << " " << mm2 << " " << line.size() <<  endl;
            
            for (int k = mm; k < mm2; k++){
                ofPoint p = line[k % line.size()];
                tempLine.addVertex(p);
            }

            
            ofPoint p;
            p = line.getVertices().back();
            if (tempLine.size() > 0){
                p = tempLine.getVertices().back();
            }
            p.y = ofGetHeight()-p.y;
            

            gradientShader.setUniform3f("posA", line.getBoundingBox().getTopLeft() * 0.5   );
            gradientShader.setUniform3f("posB", p  ); //line.getBoundingBox().getBottomRight() * 0.5 );
            gradientShader.setUniform3f("colorA",  ofPoint(0,0,0));
            gradientShader.setUniform3f("colorB",  ofPoint(1,1,1));
            gradientShader.setUniform1f("time", ofGetElapsedTimef());
            gradientShader.setUniform1f("iVal", i);
            gradientShader.setUniform1f("jVal", j);

            gradientShader.setUniform1f("rectArea", line.getBoundingBox().getArea());
            gradientShader.setUniform1f("isLine", 0.0);
            

            ofSetColor(ofRandom(0,255));

            mesh.draw();
            
            gradientShader.setUniform1f("isLine", 1.0);
            
            ofSetColor(255);
            //line.draw();
            
            tempLine.draw();
            
        
        }
        
    }
    
    img.unbind();
    
    gradientShader.end();
    
    
       
    
//    cam.begin();
//    
//    ofEnableDepthTest();
//    cam.enableOrtho();
//    //ofTranslate(-img.getWidth()/2, -img.getHeight()/2);
//    ofScale(0.5, -0.5);
//    
//    
//    gradientShader.begin();
//    gradientShader.setUniform1f("windowHeight", ofGetHeight());
//    
//    
//    img.bind();
//    
//    gradientShader.setUniformTexture("img", img, 0);
//    
//  float ff = ofGetElapsedTimef();
//    
//    
//    
//    
//    
//    for (int i = 0; i < meshes.size(); i++){
//        
//        ofPushMatrix();
//        ofTranslate(0, 0, i*3);
//        for (int j = 0; j < meshes[i].size(); j++){
//            auto & mesh = meshes[i][j];
//            auto & line = lines[i][j];
//            
//            float pct = fmod(ff*0.1 + i*0.01 + j*0.01, 1.0);
//            ofPoint p = line.getPointAtPercent(pct);
//            
//            
//            
//            gradientShader.setUniform3f("posA", line.getBoundingBox().getTopLeft() * 0.5   );
//            gradientShader.setUniform3f("posB", cam.worldToScreen( ofPoint(mouseX, mouseY)) - 0.5*ofPoint(1920/2, 1280/2)); //line.getBoundingBox().getBottomRight() * 0.5 );
//            gradientShader.setUniform3f("colorA",  ofPoint(0,0,0));
//            gradientShader.setUniform3f("colorB",  ofPoint(1,1,1));
//            gradientShader.setUniform1f("time", ofGetElapsedTimef());
//            gradientShader.setUniform1f("iVal", i);
//            gradientShader.setUniform1f("jVal", j);
//            
//            gradientShader.setUniform1f("rectArea", line.getBoundingBox().getArea());
//            
//            
//            
//            
//            mesh.draw();
//           
//        
//        }
//        ofPopMatrix();
//    }
//    
//    img.unbind();
//    
//    gradientShader.end();
//    
//    
//    for (int i = 0; i < meshes.size(); i++){
//        
//        ofPushMatrix();
//        ofTranslate(0, 0, i*3);
//
//        for (int j = 0; j < meshes[i].size(); j++){
//            auto & mesh = meshes[i][j];
//            auto & line = lines[i][j];
//            //line.draw();
//            float pct = fmod(ff*0.4 + i*0.01 + j*0.01, 1.0);
//    
//            float startPct = pct - 0.03;
//            float endPt = pct+0.03;
////
//            ofPolyline pp;
//            for (int z = 0; z < 60; z++){
//                float pctMe = ofMap(z,0,59, startPct, endPt);
//                ofPoint eh = lines[i][j].getPointAtPercent(pctMe);
//                pp.addVertex( eh );
//            }
//            pp.draw();
////
//            
//            
////            for (int z = 0; z < 180;
//            
//        }
//        ofPopMatrix();
//    }
//    
//    
//    cam.end();
    
//     for (auto & meshSet : meshes){
//      
//         for (auto & mesh : meshSet){
//             //ofSetColor(ofRandom(255));
//             
//             
////             gradientShader.setUniform1f("minX" , );
////             
////             
////             uniform vec3 colorA;
////             uniform vec3 colorB;
////             
////             uniform vec3 posA;
////             uniform vec3 posB;
////             
////             uniform float minX;
////             uniform float maxX;
////             uniform float minY;
////             uniform float maxY;
////             uniform float windowHeight;
////             uniform float windowWidth;
//
//             
//             
//             
//            // mesh.draw();
//         }
//     }
    
    
    
    ofDisableDepthTest();
    panel.draw();
    
    
    ofDrawBitmapStringHighlight(ofToString(timeMod) + "\n" + ofToString(timePct), ofPoint(700,50));
 
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
