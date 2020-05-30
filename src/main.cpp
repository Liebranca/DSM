/*********************************************************
#	INQUISITOR - CRK MODEL VIEWER                        #
#	BY LIEBRANCA                                         #
#	                                                     #
*********************************************************/

#include <iostream>

#include "backbone/display.h"
#include "backbone/fbo.h"

#include "rend/importcrk.h"
#include "rend/joj.h"

#include "rend/shader.h"
#include "rend/texture.h"
#include "lymath/transform.h"
#include "lymath/time.h"

#include "game/camera.h"
#include "game/spawnman.h"
#include "game/worldman.h"

#define CSS const std::string&
int WW = 1366; int CX = int(WW / 2);
int WH = 768; int CY = int(WH / 2);

int main(int argc, char* argv[])
{
	//--PATH-SOLVING-----------------------------------
	std::string root = std::string(argv[0]);
	std::string caller = "!!"; int l = root.length();
	root.erase(root.rfind("\\"), l); root += "\\";

	if (argc > 1) {
		caller = std::string(argv[1]);
	}

	reyn->init(root);
	std::string filename = "actors\\ura0";
		
	//--CORE-INIT--------------------------------------
	DS_DISPLAY core("Inquisitor", WW, WH);
	paint::FBO* fbo = paint::createColorBuffer();
	paint::initCanvas();
	
	DS_SHADER* screenshader = new DS_SHADER(reyn->shdpath() + "screenspace");
	
	screenshader->bind();
	glUniform1i(screenshader->getULoc("screenTexture"), 0);

	if (!jojscale.size()) { buildColorSpectrum(); }
	DS_CAMERA cam(glm::vec3(0, 1.65f, -0.5f), 90.0f, (float)WW / (float)WH, 0.01f, 100.0f);
	active_camera = &cam;

	DS_LIGHT* testlight = new DS_LIGHT({ 0,1.0f,0 }, { 1,1,1,1 }, 1.0f);
	DS_LIGHT* plyrlight = new DS_LIGHT({ 0, 0, 0 }, { 0.1f,0.1f,0.1f,1 }, 1.0f);
	Lights.push_back(testlight); Lights.push_back(plyrlight);

	std::vector<RAYDATA> lightRays; std::vector<float> lightRaysDist;
	skyColor = { 0, 0, 0, 1 };
	
	ct = new CELLTREE();
	for (unsigned int i = 0; i < scene_objects.size(); i++) {
		ct->locateObject(scene_objects[i]);
		scene_objects[i]->onFrameStart(); scene_objects[i]->update();
	}		
	
	if (caller != "!!") {
		caller.erase(0, reyn->modpath().length());
		filename = caller.substr(0, caller.find("."));
	}

	bool fwd, back, left, right, up, down, use;
	glm::vec3 mvec;
	core.mouseWrap();

	DS_NODE* ob = nullptr; DS_NODE* other = nullptr;
	RAYDATA camray, raytest; float camray_dist = 666.0f, camray_newdist = 0.0f;
	
	//--TESTING----------------------------------------
	og = spawnOnce(filename, { 0,0,-2 });

	spawnOnce("mapping\\nc_arch", { 0,4,4 });
	spawnOnce("mapping\\nc_wallend", { 2.5,0,4 });
	spawnOnce("mapping\\nc_wall", { 4,0,4 });
	spawnOnce("mapping\\nc_wall", { 6,0,4 });
	spawnOnce("mapping\\nc_wallend", { 7.5,0,4 }, { 0,0,180,0 });
	spawnOnce("mapping\\nc_wallend", { -2.5,0,4 }, { 0,0,180,0 });
	spawnOnce("mapping\\nc_wall", { -4,0,4 }, { 0,0,180,0 });
	spawnOnce("mapping\\nc_wall", { -6,0,4 }, { 0,0,180,0 });
	spawnOnce("mapping\\nc_wallend", { -7.5,0,4 });
	spawnOnce("mapping\\nc_gateR",
		{ -1,0,4 }, { 0,0,180,0 })->setTwins(spawnOnce("mapping\\nc_gate", { 1,0,4 }));

	//--MAIN-LOOP--------------------------------------
	while (!core.getIsClosed())
	{	
		//--START-BLOCK------------------------------------
		reyn->worldClock->frameStart();
		core.pollEvents();
		cam.getFrustum();

		actcam_fwd = cam.getFwd();
		actcam_pos = glm::vec4(cam.getPos(), 1);
		fuck = fBy(1.0f);
		
		//--INPUT-HANDLER----------------------------------		
		mvec = { 0, og->getAccel().y, 0 };
		fwd = core.pkeys[0]; back = core.pkeys[1];
		left = core.pkeys[2]; right = core.pkeys[3];
		up = core.pkeys[4]; down = core.pkeys[5];
		use = core.pkeys[6] && !core.helkeys[0];	

		if (fwd || back) {
			if (fwd) { og->togcomm(0); }
			else { og->togcomm(1); }
		}
		if (left || right) {
			if (left) { og->togcomm(2); }
			else { og->togcomm(3); }
		}
		if (up && og->allowJump()) { og->togcomm(4); }
		if (use && og->getInSight()) {
			og->getInSight()->useToggle();
			core.helkeys[0] = true;
		}

		if (mvec.x || mvec.z) { cam.move(mvec, true); }

		//--OBJECT-UPDATE----------------------------------
		std::vector<std::vector<DS_NODE*>> nearbyObjects;
		std::vector<DS_LIGHT*> og_nearbyLights;

		unsigned int obsize = scene_objects.size(); bool motCheck, foundGround;
		lymath::bf groundCheck;

		actcam_viewproj = cam.getViewProjection();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(skyColor[0] * 4, skyColor[1] * 4, skyColor[2] * 4, skyColor[3] * 4);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camray.hitObject = false; camray_dist = 666.0f; camray_newdist = 0.0f;
		
		for (unsigned int i = 0; i < Lights.size(); i++) { ct->locateLight(Lights[i]); }

		for (unsigned int obs = 0; obs < obsize; obs++) {
			ob = scene_objects[obs]; motCheck = true; foundGround = false;
			nearbyObjects = ct->locateObject(ob);
			nearbyLights = ct->getNearbyLights(ob);

			ob->onFrameStart();
			groundCheck = { false, -666.0f };

			if (ob->isMoving() || ob == og) {
				
				if (ob == og) {
					og_nearbyLights = nearbyLights;
					lightRays.clear(); lightRaysDist.clear();
					for (unsigned int i = 0; i < nearbyLights.size(); i++) {
						lightRays.push_back(RAYDATA()); lightRays.push_back(RAYDATA());
						lightRaysDist.push_back(0.0f); lightRaysDist.push_back(666.0f);
					}
				}

				for (unsigned int novi = 0; novi < nearbyObjects.size(); novi++) {
					for (unsigned int obm = 0; obm < nearbyObjects[novi].size(); obm++) {
						other = nearbyObjects[novi][obm];
						if (ob != other) { 
							ob->physicsSim(other, groundCheck, motCheck, foundGround);

							// CAMERA RAY
							if (ob == og) {
								raytest = ob->raytest_alt(other, cam.getFwdCast(), cam.getPos());
								if (other->usable) {
									if (raytest.hitObject) {
										camray_newdist = glm::distance(raytest.point, cam.getFwd());
										if (camray_newdist < camray_dist) {
											camray = raytest;
											camray_dist = camray_newdist;
											ob->setInSight(other);
										}
									}
								}

								if(!other->isOpenDoor) {
									for (unsigned int i = 0; i < lightRays.size(); i += 2) {
										lightRays[i] = ob->raytest_alt(other,
											nearbyLights[i / 2]->worldPosition(),
											og->worldPosition());

										if (lightRays[i].hitObject
											&& other->pointInside(lightRays[i].point)) {
											lightRaysDist[i] = glm::distance(
												lightRays[i].point, nearbyLights[i/2]->worldPosition());
											if (lightRaysDist[i] < lightRaysDist[i+1]) {
												lightRays[i+1] = lightRays[i];
												lightRaysDist[i + 1] = lightRaysDist[i];
											}
										}
									}
								}

							}
						}
					}
				}
			}

			if (ob->getTarget()) { ob->moveToTarget(); }
			if (!ob->isMapNode() && reyn->rugPulled) { ob->move(reyn->rugVec); }
			ob->update();  ob->onFrameEnd();

		}

		for (unsigned int i = 0; i < lightRays.size(); i += 2) {
			if (lightRays[i + 1].hitObject) { og_nearbyLights[i / 2]->obstruct(); }
			else if (og_nearbyLights[i / 2]->obstructed) { og_nearbyLights[i / 2]->restore(); }
		}

		/*glBindFramebuffer(GL_FRAMEBUFFER, fbo->loc);
		glClearColor(skyColor[0] * 4, skyColor[1] * 4, skyColor[2] * 4, skyColor[3] * 4);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		screenshader->bind();
		fbo->color->bind(0);

		glBindVertexArray(paint::canvas->vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);*/

		if (reyn->rugPulled) {
			for (unsigned int i = 0; i < NUM_LIGHTS; i++) {
				Lights[i]->worldPosition() += reyn->rugVec;
			}
		}
		meWorld::rugPull();

		//--CAMERA-BLOCK-----------------------------------
		if ((core.mousePos[0] == CX && core.mousePos[1] == CY) || core.mouseIgnore) {
			core.mouseRel = { 0,0 };
			core.mouseIgnore = false;
		}
		if (core.mouseRel[0] || core.mouseRel[1]) {
			float camx = -(core.mouseRel[0] * fuck * 0.5f);
			float camy = -(core.mouseRel[1] * fuck * 0.5f);
			glm::vec3 twat = glm::vec3(camx, camy, 0);

			cam.rotate(twat);
			//og->rotate(glm::vec3(0, camx, 0));
			core.mouseWrap();
		}

		cam.getPos().x = og->worldPosition().x;
		cam.getPos().y = og->worldPosition().y+2.25f;
		cam.getPos().z = og->worldPosition().z;

		glm::vec3 asscrack(-0.25f, 0, -1.65f);
		if (cam.getFwd().y < 0.0) { asscrack.z += 1.65f * abs(cam.getFwd().y); }
		else if (cam.getFwd().y > 0.0) { asscrack.y -= 2.25f * cam.getFwd().y; }

		cam.move(asscrack, true);
		plyrlight->worldPosition() = cam.getPos();

		if (og->getInSight()) {
			if (!camray.hitObject) { og->forgetInSight(); }
		}
			

		//--END-BLOCK--------------------------------------
		core.swapBuffers();
		reyn->worldClock->frameEnd();

	}
	//-------------------------------------------------
	
	delete screenshader;
	paint::deleteFrameBuffers();

	delete ct;
	delete reyn;
	
	for (unsigned int i = 0; i < scene_objects.size(); i++) {
		delete scene_objects[i];
	}

	//TODO: SORT OUT LIGHT DELETION
	for (unsigned int i = 0; i < NUM_LIGHTS; i++) {
		delete Lights[i]; NUM_LIGHTS--;
	}

	return 0;

};
