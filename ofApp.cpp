#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofBackground(39);
	ofSetWindowTitle("Insta");

	ofEnableDepthTest();

	// Initialize ofxBulletWorld
	this->cam.setPosition(ofVec3f(0, 3.f, -30.f));
	this->cam.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));
	this->world.setup();
	this->world.enableGrabbing();
	this->world.setCamera(&this->cam);
	this->world.setGravity(ofVec3f());

	// Create Big Box
	ofVec3f start_location;
	ofPoint dimens;
	float bounds_width = 400;
	float h_width = bounds_width * 0.5;
	float depth = 10.;
	float h_depth = depth * 0.5;
	this->bounds_shape.reset(new ofxBulletCustomShape());
	this->bounds_shape->create(world.world, ofVec3f(0, 0, 0), 10.);
	for (int i = 0; i < 6; i++) {
		if (i == 0) { // ground //
			start_location.set(0., h_width + h_depth, 0.);
			dimens.set(bounds_width, depth, bounds_width);
		}
		else if (i == 1) { // back wall //
			start_location.set(0, 0, h_width + h_depth);
			dimens.set(bounds_width, bounds_width, depth);
		}
		else if (i == 2) { // right wall //
			start_location.set(h_width + h_depth, 0, 0.);
			dimens.set(depth, bounds_width, bounds_width);
		}
		else if (i == 3) { // left wall //
			start_location.set(-h_width - h_depth, 0, 0.);
			dimens.set(depth, bounds_width, bounds_width);
		}
		else if (i == 4) { // ceiling //
			start_location.set(0, -h_width - h_depth, 0.);
			dimens.set(bounds_width, depth, bounds_width);
		}
		else if (i == 5) { // front wall //
			start_location.set(0, 0, -h_width - h_depth);
			dimens.set(bounds_width, bounds_width, depth);
		}
		btBoxShape* boxShape = ofBtGetBoxCollisionShape(dimens.x, dimens.y, dimens.z);
		this->bounds_shape->addShape(boxShape, start_location);
	}
	this->bounds_shape->add();

	// Create Spheres
	for (int i = 0; i < 255; i++) {

		unique_ptr<ofxBulletSphere> sphere(new ofxBulletSphere);
		float size = 5;
		sphere->create(this->world.world, ofVec3f(ofRandom(-100, 100), ofRandom(-100, 100), ofRandom(-100, 100)), 1, size);
		sphere->setRestitution(1.0);
		sphere->add();
		this->spheres.push_back(move(sphere));
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	// Big Box Moving To Center 
	if (!ofGetMousePressed()) {

		ofVec3f diff = ofVec3f(0, 0, 0) - bounds_shape->getPosition();
		diff *= 500.f;
		bounds_shape->applyCentralForce(diff);
	}

	// Compute Spheres Relation
	for (int i = 0; i < this->spheres.size(); i++) {

		for (int j = i + 1; j < this->spheres.size(); j++) {

			float distance = this->spheres[i]->getPosition().distance(this->spheres[j]->getPosition());
			if (distance < 80) {

				ofVec3f diff = this->spheres[i]->getPosition() - this->spheres[j]->getPosition();
				ofMap(distance, 0, 50, 40, -2);
				this->spheres[i]->applyCentralForce(diff);
				this->spheres[j]->applyCentralForce(-diff);
			}
		}
	}

	this->world.update();
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

	// Draw Big Box
	ofNoFill();
	if (ofGetMousePressed()) {

		ofSetColor(39, 39, 239);
	}
	else {

		ofSetColor(239);
	}
	bounds_shape->transformGL();
	ofDrawBox(ofVec3f(0, 0, 0), 400);
	bounds_shape->restoreTransformGL();

	// Draw Sphere
	ofFill();
	for (int i = 0; i < this->spheres.size(); i++) {

		for (int j = i + 1; j < this->spheres.size(); j++) {

			float distance = this->spheres[i]->getPosition().distance(this->spheres[j]->getPosition());
			if (distance < 30) {

				ofSetColor(239);
				ofDrawLine(this->spheres[i]->getPosition(), this->spheres[j]->getPosition());
				ofDrawSphere(this->spheres[i]->getPosition(), this->spheres[i]->getRadius());
				ofDrawSphere(this->spheres[j]->getPosition(), this->spheres[j]->getRadius());
			}
			else if (distance < 60) {

				ofSetColor(239, ofMap(distance, 30, 60, 255, 0));
				ofDrawLine(this->spheres[i]->getPosition(), this->spheres[j]->getPosition());
				ofDrawSphere(this->spheres[i]->getPosition(), this->spheres[i]->getRadius());
				ofDrawSphere(this->spheres[j]->getPosition(), this->spheres[j]->getRadius());
			}
		}
	}
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}