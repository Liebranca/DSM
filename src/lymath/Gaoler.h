#ifndef GAOLER_H
#define GAOLER_H

#include <iostream>
#include <vector>
#include <string>
#include "../lymath/utils.h"

extern bool debug_drawBounds;
extern float gaol_colMargin;
extern float gaol_physClamp;
extern float gaol_physDeacc;

struct RAYDATA {
	bool hitObject = false;
	glm::vec3 point = {0,0,0};
};

class COLDATA {

	public:

		// for plane collision
		COLDATA(glm::vec3 colnormal, glm::vec3& colpoint) {
			this->colnormal = colnormal; this->colpoint;
		}

		virtual ~COLDATA() { ; }

		glm::vec3 colnormal;
		glm::vec3 colpoint;

};

class COLLINE {

public:

	COLLINE(glm::vec3 p1, glm::vec3 p2) {
		this->p1 = p1; this->p2 = p2; this->normal = (p2 - p1);
		this->centroid = (p1 + p2) / 2.0f;
		this->length = lymath::vec3len(this->normal); this->normal = glm::normalize(this->normal);
	}

	glm::vec3 pointAlong(float s) {
		return p1 + ((this->normal * this->length) * s);
	}

	virtual ~COLLINE() { ; }

	float length = 0.0f;
	glm::vec3 p1 = ZVEC3, p2 = ZVEC3, normal = ZVEC3, centroid = ZVEC3;

};

class COLFACE {

	public:
		COLFACE(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, std::string name = ""){
			this->centroid = (p1 + p3) / 2.0f;
			this-> normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
			this->d = glm::dot(-this->centroid, normal);

			this->edges.reserve(2 * sizeof(COLLINE));
			this->edges = { COLLINE(p1, p2), COLLINE(p2, p3) };
			this->name = name;

			this->p4 = p3 + glm::vec3(p1.x, 0, p1.z);
			
		}

		virtual ~COLFACE() { ; }

		COLDATA planeIsect(COLFACE& p) {

			glm::vec3 inormal = glm::cross(this->normal, p.normal);
			float det = pow(glm::length(inormal), 2);

			if (det > 0.001f) {
				glm::vec3 ipoint = ((glm::cross(inormal, p.normal) * this->d) +
									(glm::cross(this->normal, inormal) * p.d)) / det;

				return COLDATA(p.normal, ipoint);
			}
			
			return COLDATA(p.normal, p.centroid);

		}

		bool edgeIsect(COLFACE& p) {

			if (this->edges.size() != 2 || p.edges.size() != 2) { return false; }

			float dx = lymath::codist(this->centroid.x, p.centroid.x);
			float dy = lymath::codist(this->centroid.y, p.centroid.y);
			float dz = lymath::codist(this->centroid.z, p.centroid.z);

			float l1 = (this->edges[0].length + p.edges[0].length);
			float l2 = (this->edges[1].length + p.edges[1].length);
			
			bool b1 = dx+dz < l1;
			bool b2 = dy < l2;

			return b1 && b2;
		}

		RAYDATA rayIsectPoint(COLLINE ray) {
			float denom = dot(this->normal, ray.normal);
			if (abs(denom) <= 1e-4f) { return { false, {0,0,0} }; }
			float t = -(dot(this->normal, ray.p1) + this->d) / denom;
			if (t <= 1e-4) { return { false, {0,0,0} }; }
			return { true, ray.p1 + t * ray.normal };
		}

		glm::vec3 centroid, normal, p4;
		std::string name;
		std::vector<COLLINE> edges;
		float d;

};

class COLBOX {
	
	public:
		COLBOX(std::vector<glm::vec3> p) {
			faces.reserve(6 * sizeof(COLFACE));
			this->faces = { COLFACE(p[0], p[4], p[7], "bottom"), COLFACE(p[2], p[1], p[5], "top"),
					  COLFACE(p[0], p[4], p[5], "front"), COLFACE(p[2], p[6], p[7], "back"),
					  COLFACE(p[5], p[6], p[7], "right"), COLFACE(p[1], p[2], p[3], "left") };

			this->faces[0].normal = -this->faces[0].normal;
			this->faces[0].d = -this->faces[0].d;

			this->faces[4].normal = -this->faces[4].normal;
			this->faces[4].d = -this->faces[4].d;

			this->area[0] = lymath::gmaxf(this->faces[0].edges[0].length, this->faces[0].edges[1].length);
			this->area[1] = this->faces[2].edges[1].length;

			this->points = p;
		}

		virtual ~COLBOX() { ; }

		std::vector<glm::vec3> points;
		std::vector<COLFACE> faces;
		float area[2];

		COLBOX selfProjection(glm::vec3 dirn) {
			std::vector<glm::vec3> projected_points;
			for (unsigned int i = 0; i < this->points.size(); i++) {
				projected_points.push_back(this->points[i] + dirn);
			}

			return COLBOX(projected_points);
		}
		std::vector<RAYDATA> genCage(COLFACE other) {
			std::vector<RAYDATA> cage =
			{
				other.rayIsectPoint(this->faces[0].edges[0]),
				other.rayIsectPoint(this->faces[0].edges[1]),
				other.rayIsectPoint(COLLINE(this->faces[0].edges[1].p2, this->faces[0].p4)),
				other.rayIsectPoint(COLLINE(this->faces[0].p4, this->faces[0].edges[0].p1)),

				other.rayIsectPoint(this->faces[1].edges[0]),
				other.rayIsectPoint(this->faces[1].edges[1]),
				other.rayIsectPoint(COLLINE(this->faces[1].edges[1].p2, this->faces[1].p4)),
				other.rayIsectPoint(COLLINE(this->faces[1].p4, this->faces[1].edges[0].p1)),

				other.rayIsectPoint(this->faces[2].edges[1]),
				other.rayIsectPoint(this->faces[3].edges[1]),
				other.rayIsectPoint(this->faces[4].edges[1]),
				other.rayIsectPoint(this->faces[5].edges[1])
			};

			return cage;
		}

		bool bottomFaceCol(COLFACE other, float fac) {
			glm::vec3 vel(0, fac, 0);
			if (other.rayIsectPoint(
				COLLINE(this->faces[0].edges[0].p1, this->faces[0].edges[0].p1 - vel)).hitObject) {
				return true;
			}
			else if (other.rayIsectPoint(
				COLLINE(this->faces[0].edges[0].p2, this->faces[0].edges[0].p2 - vel)).hitObject) {
				return true;
			}
			else if (other.rayIsectPoint(
				COLLINE(this->faces[0].edges[1].p2, this->faces[0].edges[1].p2 - vel)).hitObject) {
				return true;
			}
			else if (other.rayIsectPoint(
				COLLINE(this->faces[0].p4, this->faces[0].p4 - vel)).hitObject) {
				return true;
			}

			return false;
		}

		RAYDATA raytest(COLBOX* other, COLLINE ray) {
			RAYDATA rd, test;

			for (int i = 2; i < 6; i++) {
				test = other->faces[i].rayIsectPoint(ray);
				if (test.hitObject) { rd = test; break; }
			}
			if (rd.hitObject) { 
				if (glm::distance(rd.point, ray.centroid) > ray.length / 2) { rd.hitObject = false; }
			}
			return rd;

		}

		lymath::bf groundCheck(COLBOX* other, float fac) {
			if (!this->inDomainOf(other)) { return { false, 0.0f }; }
			lymath::bf gchk = { false, {0.0f} };
			for (int i = 0; i < 6; i++) {
				if (dot(this->faces[0].normal, -other->faces[i].normal) > 0.075f) {
					if (this->bottomFaceCol(other->faces[i], fac)) {
						gchk = { true, other->faces[i].centroid.y }; break;
					}
				}
			} return gchk;
		}

		bool pointInside(glm::vec3 point) {
			unsigned int count = 0;
			for (int i = 0; i < 6; i++) {
				if ((dot(faces[i].normal, point) + faces[i].d) <= gaol_colMargin) { count++; }
			}
			return count == 6;

		}

		bool cageIsect(std::vector<RAYDATA> rayarray) {
			for (unsigned int i = 0; i < rayarray.size(); i++) {
				if (rayarray[i].hitObject) {
					if (this->pointInside(rayarray[i].point)) { return true; }
				}
			} return false;
		}

		bool lineInside(COLLINE& ray) {
			glm::vec3 p;
			for (int i = 0; i < int(ray.length); i++) {
				p = ray.pointAlong(i * 0.1f);
				if (this->pointInside(p)) { return true; }
			} return false;
		}

		bool hasPointInDomain(glm::vec3 v) {
			glm::vec3& d0 = this->faces[0].edges[0].p1;
			glm::vec3& d1 = this->faces[0].edges[1].p2;

			float bdxm = lymath::gminf(d0.x, d1.x) - gaol_colMargin;
			float bdxp = lymath::gmaxf(d0.x, d1.x) + gaol_colMargin;
			float bdym = lymath::gminf(d0.y, d1.y) - gaol_colMargin;
			float bdyp = lymath::gmaxf(d0.y, d1.y) + gaol_colMargin;
			float bdzm = lymath::gminf(d0.z, d1.z) - gaol_colMargin;
			float bdzp = lymath::gmaxf(d0.z, d1.z) + gaol_colMargin;

			bool in_xdom = (bdxm <= v.x && v.x <= bdxp);
			bool in_ydom = (bdym <= v.y && v.y <= bdyp);
			bool in_zdom = (bdzm <= v.z && v.z <= bdzp);

			return in_xdom && in_zdom;

		}

		bool inDomainOf(COLBOX* b) {
			glm::vec3& d0 = b->faces[0].edges[0].p1;
			glm::vec3& d1 = b->faces[0].edges[1].p2;
			float bdxm = lymath::gminf(d0.x, d1.x), bdxp = lymath::gmaxf(d0.x, d1.x);
			float bdzm = lymath::gminf(d0.z, d1.z), bdzp = lymath::gmaxf(d0.z, d1.z);

			d0 = this->faces[0].edges[0].p1;
			d1 = this->faces[0].edges[1].p2;
			float tdxm = lymath::gminf(d0.x, d1.x), tdxp = lymath::gmaxf(d0.x, d1.x);
			float tdzm = lymath::gminf(d0.z, d1.z), tdzp = lymath::gmaxf(d0.z, d1.z);

			bool in_xdom = (bdxm <= tdxm && tdxp <= bdxp) || (bdxm <= tdxp && tdxm <= bdxp);
			bool in_zdom = (bdzm <= tdzm && tdzp <= bdzp) || (bdzm <= tdzp && tdzm <= bdzp);
			
			return in_xdom && in_zdom;

		}

		glm::vec3 boxToBox(COLBOX* b, glm::vec3 dirn) {
			if (!this->faces.size() || !b->faces.size() || !this->inDomainOf(b)) { return { 0,0,0 }; }
			glm::vec3 n = { 0,0,0 }; bool col = false;

			for (unsigned int i = 0; i < 6; i++) {
				if (dot(dirn, -b->faces[i].normal) > 0.075f) {
					col = this->cageIsect(this->genCage(b->faces[i]));
					if (col) { n += b->faces[i].normal; break; }
				}
			} return n;

		}

};

class DS_BOUNDS {

	public:
		DS_BOUNDS(std::vector<glm::vec4>& points) {
			this->points = points;
		}

		virtual ~DS_BOUNDS() { ; }

		COLBOX* genBox(glm::mat4& model) {

			std::vector<glm::vec3> rect =
			  { glm::vec3(model * points[0]), glm::vec3(model * points[1]),
				glm::vec3(model * points[2]), glm::vec3(model * points[3]),
				glm::vec3(model * points[4]), glm::vec3(model * points[5]),
				glm::vec3(model * points[6]), glm::vec3(model * points[7]) };

			return new COLBOX(rect);

		};

		std::vector<glm::vec4> points;

};

#endif //GAOLER_H
