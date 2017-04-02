#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofDirectory dir;
    dir.listDir("2017-03-31-16-41-36-176");
    
    
    gradientShader.load("shader/gradient.vert", "shader/gradient.frag");
    
    spot.load("spot.png");
    
    img2.load("bigDepth.png");
    img.load("big.png");
    ofxCv::blur(img2,5);
    
    for (int i = 0; i < 80; i++){
        float pct =ofMap(i, 0, 80, 0, 1);
        finder.setThreshold(ofMap(powf(pct, 2.3), 0, 1, 1, 255));
        finder.setMinArea(100);
        finder.setMaxArea(img.getWidth() * img.getHeight()*0.99);
        finder.findContours(img2);
        
        vector < ofPolyline  > linesTemp;
        for (int j = 0; j < finder.size(); j++){
            linesTemp.push_back(finder.getPolyline(j).getResampledBySpacing(10).getSmoothed(3));
            
        }
        
        
            for (auto & line : linesTemp ){
                for (auto & ptrt : line ){
                    ptrt = ptrt - ofPoint(img.getWidth()/2, img.getHeight()/2);
                }

            }
        lines.push_back(linesTemp);
        

    }

    
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
    
}

//--------------------------------------------------------------
void ofApp::update(){

    if (ofGetFrameNum() % 60 == 0){
        gradientShader.load("shader/gradient.vert", "shader/gradient.frag");
    }
}


ofEasyCam cam;


//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);

    
    cout << ofGetFrameRate() << endl;
    
    
    
    vector < vector < ofMesh > > meshesProjected;
    meshesProjected = meshes;
    vector < vector < ofPolyline > > linesProjected;
    linesProjected = lines;
    
    
    ofRectangle viewport = ofGetCurrentViewport();
    
    
    
    
    
    cam.begin();
 // cam.enableOrtho();
    
    ofMatrix4x4 mat = cam.getModelViewProjectionMatrix(viewport);
    
    //-------------------------------------- project meshes
    int levelCount = 0;
    for (auto & meshSet : meshesProjected){
        for (auto & meshes : meshSet){
            for (auto & vertex : meshes.getVertices()){
             
                
                ofVec3f CameraXYZ = (vertex + ofPoint(0,0,levelCount*5))*ofPoint(0.5, -0.5, 0.5) * mat;
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
        for (auto & lines : lineSet){
            for (auto & vertex : lines.getVertices()){
            
                ofVec3f CameraXYZ = (vertex + ofPoint(0,0,levelCount*5))*ofPoint(0.5, -0.5, 0.5) * mat;
                ofVec3f ScreenXYZ;
                
                ScreenXYZ.x = (CameraXYZ.x + 1.0f) / 2.0f * viewport.width + viewport.x;
                ScreenXYZ.y = (1.0f - CameraXYZ.y) / 2.0f * viewport.height + viewport.y;
                ScreenXYZ.z = levelCount*0.1+0.01;
                
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
    
    
    
    
    gradientShader.begin();
    gradientShader.setUniform1f("windowHeight", ofGetHeight());


    img.bind();

    gradientShader.setUniformTexture("img", img, 0);

  float ff = ofGetElapsedTimef();


    for (int i = 0; i < meshesProjected.size(); i++){

        for (int j = 0; j < meshesProjected[i].size(); j++){
            auto & mesh = meshesProjected[i][j];
            auto & line = linesProjected[i][j];

            
            float len = line.getPerimeter();
            
            float pct = fmod(ff*100.0, len*1.1) / len*1.1;
            ofPoint p = line.getPointAtPercent(pct);

            //p += ofPoint(i, j);
            //line.getClosestPoint(ofPoint(mouseX, mouseY));
            
            p.y = ofGetHeight()-p.y;

            gradientShader.setUniform3f("posA", line.getBoundingBox().getTopLeft() * 0.5   );
            gradientShader.setUniform3f("posB", p); //line.getBoundingBox().getBottomRight() * 0.5 );
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
            line.draw();
        
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
