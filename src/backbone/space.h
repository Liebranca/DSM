#ifndef SPACE_H
#define SPACE_H

#include "../lymath/utils.h"
#include "../game/node.h"
#include "../rend/Light.h"

static std::vector<int> inlu(int v, int vmax, int vmin) {
	int a = v + 1, b = v - 1;
	if (a > vmax) { a = vmin; } if (b < vmin) { b = vmax; }
	return { a, b };

}

class TILE {
	
	public:
		TILE(glm::vec3 pos) {
			this->pos = pos;

		}

		virtual ~TILE() { ; }

		void occupy() { occ++; }
		void free() { occ--; }
		
		glm::vec3 pos;

	private:
		int occ = 0;

};

class NINANT {

	public:
		NINANT(glm::vec3 pos, int ix, int iz) {
			for (int i = 0; i < 3; i++) { this->pos[i] = pos[i]; }

			this->ix = ix; this->iz = iz;
			glm::vec3 tilepos;
			this->tiles.reserve(4 * sizeof(TILE));

			for (int z = 1; z > -2; z -= 2) {
				for (int x = 1; x > -2; x -= 2) {
					for (int y = 0; y < 1; y++) {

						tilepos.x = (x * 0.5f) + pos[0];
						tilepos.z = (z * 0.5f) + pos[2];
						tilepos.y = (float)y + pos[1];
						
						this->tiles.push_back(new TILE(tilepos));

					}
				}
			}
		}

		TILE* getTile(glm::vec3 tilepos) {
			int nearest = 0; float dist = 8.0f, newdist = 0.0f;
			TILE* t = nullptr;
			for (int i = 0; i < 4; i++) {
				t = this->tiles[i];
				newdist = glm::distance(tilepos, t->pos);
				if (newdist < dist) { nearest = i; dist = newdist; }

			}
			t = this->tiles[nearest];
			std::cout << "tile " << nearest << std::endl;
			return t;

		}

		virtual ~NINANT() { 
			for (unsigned int i = 0; i < this->tiles.size(); i++) {
				delete this->tiles[i]; this->tiles[i] = nullptr;
			}
		}

	private:
		std::vector<TILE*> tiles;
		glm::vec3 pos;
		int ix, iz;

};

class ENNEAD {

	public:
		ENNEAD(glm::vec3 pos, int ix, int iz, int px, int pz) {
			glm::vec3 ninpos(0, 0, 0); this->neighbors.reserve(9 * sizeof(ENNEAD*));
			std::vector<NINANT*> emp = { nullptr, nullptr, nullptr };

			this->ix = ix; this->iz = iz; this->px = px; this->pz = pz;

			for (int i = 0; i < 3; i++) {
				this->pos[i] = pos[i]; this->nins.push_back(emp);
			}

			for (int z = -1; z < 2; z++) {
				for (int x = -1; x < 2; x++) {
					ninpos[0] = (x * 2) + pos[0];
					ninpos[2] = (z * 2) + pos[2];
					ninpos[1] = pos[1];

					this->nins[x+1][z+1] = new NINANT(ninpos, x+1, z+1);

				}
			}

		}

		virtual ~ENNEAD() {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					delete this->nins[i][j]; this->nins[i][j] = nullptr;
				}
			}
		}

		NINANT* getNin(glm::vec3 pos) {
			glm::vec3 lpos = pos - this->pos;
			int nini[2] = { (int)roundf(lpos.x / 3) + 1, (int)roundf(lpos.z / 3) + 1 };
			return this->nins[nini[0]][nini[1]];
		}

		std::vector<int> getNeighborIndexList() {
			std::vector<int> nix = inlu(this->ix, 2, 0), niz = inlu(this->iz, 2, 0);
			return { nix[0], niz[0], this->ix, niz[0], nix[1], niz[0],
					 nix[0], this->iz, nix[1], this->iz,
					 nix[0], niz[1], this->ix, niz[1], nix[1], niz[1] };
		}

		void regNeighbor(ENNEAD* n) { this->neighbors.push_back(n); }

		int getNeighborNovIndex(int nix, int niz) {

			bool c0 = nix == 2 && niz == 2;
			bool c1 = nix == 1 && niz == 2;
			bool c2 = nix == 0 && niz == 2;
			bool c3 = nix == 2 && niz == 1;
			bool c4 = nix == 0 && niz == 1;
			bool c5 = nix == 2 && niz == 0;
			bool c6 = nix == 1 && niz == 0;
			bool c7 = nix == 0 && niz == 0;

			bool e0 = this->ix == 2 && this->iz == 2;
			bool e1 = this->ix == 1 && this->iz == 2;
			bool e2 = this->ix == 0 && this->iz == 2;
			bool e3 = this->ix == 2 && this->iz == 1;
			bool e4 = this->ix == 0 && this->iz == 1;
			bool e5 = this->ix == 2 && this->iz == 0;
			bool e6 = this->ix == 1 && this->iz == 0;
			bool e7 = this->ix == 0 && this->iz == 0;

			bool up = (e0 || e1 || e2) && (c5 || c6 || c7);
			bool down = (e5 || e6 || e7) && (c0 || c1 || c2) && !up;
			bool left = (e0 || e3 || e5) && (c2 || c4 || c7);
			bool right = (e2 || e4 || e7) && (c0 || c3 || c5) && !left;

			if (up && left) { return 0; }
			if (down && right) { return 7; }
			if (up && right) { return 2; }
			if (down && left) { return 5; }

			if (up) { return 1; }
			if (down) { return 6; }
			if (left) { return 3; }
			if (right) { return 4; }
				
			return 5;

			/*
					0	1	1	1	2

			0 1 2	3	22 12 02	4
			3 # 4	3	21 11 01	4
			5 6 7	3	20 10 00	4

					5	6	6	6	7
			*/		

		}

		bool isCenterEnn() { return this->ix == 1 && this->iz == 1; }
		bool isNeighborOuter(int index) { 
				return (this->neighbors[index]->px != this->px
				|| this->neighbors[index]->pz != this->pz);
		}

		ENNEAD* getNeighbor(int index) { return this->neighbors[index]; }
		unsigned int numNeighbors() { return this->neighbors.size(); }

		void regOb(DS_NODE* ob) {
			std::vector<int> oldenn;
			bool ennchange = ob->ennCheck(this->ix, this->iz);
			if (ennchange) {
				oldenn = ob->onEnnChange(this->ix, this->iz);
			}
		}

		void regLight(DS_LIGHT* ob) {
			std::vector<int> oldenn;
			bool ennchange = ob->ennCheck(this->ix, this->iz);
			if (ennchange) {
				oldenn = ob->onEnnChange(this->ix, this->iz);
			}
		}

		void printOf() {
			ENNEAD* a;
			for (unsigned int i = 0; i < this->neighbors.size(); i++) {
				if (this->neighbors[i]) {
					a = this->neighbors[i];
					std::cout << a->ix << ", " << a->iz << " | ";
					if (i == 3) { std::cout << "x, x | "; }
					if (i == 2 || i == 4 || i == 7) { std::cout << "\n"; }
				}
			}std::cout << "\n______________\n\n";
		}

		int px, pz;

	private:
		std::vector<ENNEAD*> neighbors;
		std::vector<std::vector<NINANT*>> nins;
		glm::vec3 pos;
		int ix, iz;

};

class NOVELL {

	public:
		NOVELL(glm::vec3 pos, int ix, int iz, int px, int pz) {
			std::vector<int> dom; glm::vec3 ennpos(0,0,0);
			dom.reserve(2 * sizeof(int)); dom = { 0, 0 };
			this->enns.reserve(9 * sizeof(ENNEAD));
			this->neighbors.reserve(9 * sizeof(NOVELL*));

			this->ix = ix; this->iz = iz; this->px = px; this->pz = pz;

			for (int i = 0; i < 3; i++) {
				this->pos[i] = pos[i];
			}

			DS_NODE* ground;
			bool ass = true;
			std::vector<ENNEAD*> emp = { nullptr, nullptr, nullptr };
			for (int i = 0; i < 3; i++) { this->enns.push_back(emp); }

			for (int z = -1; z < 2; z++) {
				for (int x = -1; x < 2; x++) {
					ennpos[0] = (x * 8.0f) + pos[0];
					ennpos[2] = (z * 8.0f) + pos[2];
					ennpos[1] = pos[1];

					this->enns[x+1][z+1] = new ENNEAD(ennpos, x+1, z+1, px, pz);

					ground = new DS_NODE("mapping\\nc_floorA");
					ground->worldPosition() = glm::vec3(
						(float)ennpos[0], (float)ennpos[1], (float)ennpos[2]);

				}
			}
		}

		virtual ~NOVELL() { 
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					delete this->enns[i][j]; this->enns[i][j] = nullptr;
				}
			}
		}

		ENNEAD* ennByIdex(int x, int z) {
			return this->enns[x][z];
		}

		ENNEAD* getEnn(glm::vec3 pos) { 
			glm::vec3 lpos = pos - this->pos;
			int enni[2] = { (int)roundf(lpos.x / 8) + 1, (int)roundf(lpos.z / 8) + 1 };
			return this->enns[enni[0]][enni[1]];
		}

		void getEnnNeighbors() {
			ENNEAD* enn = nullptr; ENNEAD* enn2 = nullptr; int nix, niz, novi;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					enn = this->enns[i][j];

					std::vector<int> ni = enn->getNeighborIndexList();
					for (int xx = 0; xx < 15; xx += 2) {
						nix = ni[xx]; niz = ni[xx + 1];
						if (!enn->isCenterEnn()) {
							novi = enn->getNeighborNovIndex(nix, niz);
						}
						else { novi = -1; }
						
						if (novi != -1) {
							if (this->neighbors[novi]) {
								enn->regNeighbor(this->neighbors[novi]->enns[nix][niz]);
							}
							else { enn->regNeighbor(nullptr); }
						}
						else { enn->regNeighbor(this->enns[nix][niz]); }

					}
				}
			}
		}

		bool checkNeighbor(int nix, int niz) {
			bool bx = (nix == 4 && this->ix == 0) || (nix == 0 && this->ix == 4);
			bool bz = (niz == 4 && this->iz == 0) || (niz == 0 && this->iz == 4);
			
			if (bx || bz) { return false; } return true;

		}

		void regNeighbor(NOVELL* nov) { this->neighbors.push_back(nov); }

		std::vector<int> getNeighborIndexList(int i, int j) {
			std::vector<int> nix = inlu(i, 4, 0), niz = inlu(j, 4, 0);
			return { nix[0], niz[0], i, niz[0], nix[1], niz[0],
					 nix[0], j, nix[1], j,
					 nix[0], niz[1], i, niz[1], nix[1], niz[1] };
		}

		unsigned int numNeighbors() { return this->neighbors.size(); }

		std::vector<int> regOb(DS_NODE* ob) {
			std::vector<int> oldnov;
			bool novchange = ob->novCheck(this->px, this->pz);
			if (novchange) {
				oldnov = ob->onNovChange(this->px, this->pz);
			}
			std::vector<DS_NODE*>::iterator pp = std::find(this->occ.begin(), this->occ.end(), ob);
			if (pp == this->occ.end()) {
				this->occ.push_back(ob);
			}
			return oldnov;
		}

		std::vector<int> regLight(DS_LIGHT* ob) {
			std::vector<int> oldnov;
			bool novchange = ob->novCheck(this->px, this->pz);
			if (novchange) {
				oldnov = ob->onNovChange(this->px, this->pz);
			}
			std::vector<DS_LIGHT*>::iterator pp = std::find(this->occ_l.begin(), this->occ_l.end(), ob);
			if (pp == this->occ_l.end()) {
				this->occ_l.push_back(ob);
			}
			return oldnov;
		}

		void delOb(DS_NODE* ob) {
			std::vector<DS_NODE*>::iterator pp = std::find(this->occ.begin(), this->occ.end(), ob);

			if (pp != this->occ.end()) {
				this->occ.erase(pp);
			}
		}

		void delLight(DS_LIGHT* ob) {
			std::vector<DS_LIGHT*>::iterator pp = std::find(this->occ_l.begin(), this->occ_l.end(), ob);

			if (pp != this->occ_l.end()) {
				this->occ_l.erase(pp);
			}
		}

		void printOf() {
			NOVELL* a;
			for (unsigned int i = 0; i < this->neighbors.size(); i++) {
				if (this->neighbors[i]) {
					a = this->neighbors[i];
					std::cout << a->px << ", " << a->pz << " | ";
				}
			} std::cout << "\n";
		}

		std::vector<DS_NODE*>& getOcc() { return this->occ; }
		std::vector<DS_LIGHT*>& getOcc_l() { return this->occ_l; }

	private:
		std::vector<DS_NODE*> occ;
		std::vector<DS_LIGHT*> occ_l;
		std::vector<NOVELL*> neighbors;
		std::vector<std::vector<ENNEAD*>> enns;
		glm::vec3 pos;
		int ix, iz, px, pz;
};

class CELLTREE {

	public:
		CELLTREE() {

			std::vector<NOVELL*> emp = { nullptr, nullptr, nullptr, nullptr, nullptr };
			for (int i = 0; i < 5; i++) { this->novs.push_back(emp); }

			this->novs[0][0] = new NOVELL({ -48,0,-48 }, -2, -2, 0, 0);
			this->novs[0][1] = new NOVELL({ -48,0,-24 }, -2, -1, 0, 1);
			this->novs[0][2] = new NOVELL({ -48,0,0 }, -2, 0, 0, 2);
			this->novs[0][3] = new NOVELL({ -48,0,24 }, -2, 1, 0, 3);
			this->novs[0][4] = new NOVELL({ -48,0,48 }, -2, 2, 0, 4);

			this->novs[1][0] = new NOVELL({ -24,0,-48 }, -1, -2, 1, 0);
			this->novs[1][1] = new NOVELL({ -24,0,-24 }, -1, -1, 1, 1);
			this->novs[1][2] = new NOVELL({ -24,0,0 }, -1, 0, 1, 2);
			this->novs[1][3] = new NOVELL({ -24,0,24 }, -1, 1, 1, 3);
			this->novs[1][4] = new NOVELL({ -24,0,48 }, -1, 2, 1, 4);

			this->novs[2][0] = new NOVELL({ 0,0,-48 }, 0, -2, 2, 0);
			this->novs[2][1] = new NOVELL({ 0,0,-24 }, 0, -1, 2, 1);
			this->novs[2][2] = new NOVELL({ 0,0,0 }, 0, 0, 2, 2);
			this->novs[2][3] = new NOVELL({ 0,0,24 }, 0, 1, 2, 3);
			this->novs[2][4] = new NOVELL({ 0,0,48 }, 0, 2, 2, 4);

			this->novs[3][0] = new NOVELL({ 24,0,-48 }, 1 , -2, 3, 0);
			this->novs[3][1] = new NOVELL({ 24,0,-24 }, 1, -1, 3, 1);
			this->novs[3][2] = new NOVELL({ 24,0,0 }, 1, 0, 3, 2);
			this->novs[3][3] = new NOVELL({ 24,0,24 }, 1, 1, 3, 3);
			this->novs[3][4] = new NOVELL({ 24,0,48 }, 1, 2, 3, 4);

			this->novs[4][0] = new NOVELL({ 48,0,-48 }, 2, -2, 4, 0);
			this->novs[4][1] = new NOVELL({ 48,0,-24 }, 2, -1, 4, 1);
			this->novs[4][2] = new NOVELL({ 48,0,0 }, 2, 0, 4, 2);
			this->novs[4][3] = new NOVELL({ 48,0,24 }, 2, 1, 4, 3);
			this->novs[4][4] = new NOVELL({ 48,0,48 }, 2, 2, 4, 4);

			//get neighboring tiles
			NOVELL* nov = nullptr; bool reg = false; int nix, niz;
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++) {
					nov = this->novs[i][j];
					std::vector<int> ni = nov->getNeighborIndexList(i, j);
					
					for (int xx = 0; xx < 15; xx += 2) {
						nix = ni[xx]; niz = ni[xx + 1];
						reg = nov->checkNeighbor(nix, niz);
						if (reg) { nov->regNeighbor(this->novs[nix][niz]); }
						else { nov->regNeighbor(nullptr); }
					}
					
					nov->getEnnNeighbors();
				}
			}

		}

		virtual ~CELLTREE() {
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++) {
					delete this->novs[i][j]; this->novs[i][j] = nullptr;
				}
			}
		}

		void printOf(DS_NODE* ob) {
			ENNEAD* enn = this->getNov(ob->worldPosition())->getEnn(ob->worldPosition());
			if (enn) { enn->printOf(); }
		}

		std::vector<std::vector<DS_NODE*>> locateObject(DS_NODE* ob) {

			std::vector<std::vector<DS_NODE*>> obj_list;
			NOVELL* neighbor = nullptr;
			NOVELL* nov = this->getNov(ob->worldPosition());
			if (!nov) { 
				//TODO: object is out of bounds -- queue deletion
				return obj_list;
			}
			
			ENNEAD* enn = nov->getEnn(ob->worldPosition());
			ENNEAD* n; enn->regOb(ob);

			std::vector<int> op = nov->regOb(ob);
			if (op.size()) {
				this->novs[op[0]][op[1]]->delOb(ob);
			}

			obj_list.push_back(nov->getOcc());
			for (unsigned int i = 0; i < enn->numNeighbors(); i++) {
				n = enn->getNeighbor(i);
				if (n) {
					if (n->isNeighborOuter(i)) {
						neighbor = this->novs[n->px][n->pz];
						std::vector<std::vector<DS_NODE*>>::iterator pp =
							std::find(obj_list.begin(), obj_list.end(), neighbor->getOcc());

						if (pp == obj_list.end()) {
							obj_list.push_back(neighbor->getOcc());
						}
					}
				}
			}

			return obj_list;

		}

		void locateLight(DS_LIGHT* ob) {

			std::vector<std::vector<DS_LIGHT*>> obj_list;
			NOVELL* neighbor = nullptr;
			NOVELL* nov = this->getNov(ob->worldPosition());
			if (!nov) {
				//TODO: light is out of bounds -- queue deletion
			}

			ENNEAD* enn = nov->getEnn(ob->worldPosition()); enn->regLight(ob);

			std::vector<int> op = nov->regLight(ob);
			if (op.size()) {
				this->novs[op[0]][op[1]]->delLight(ob);
			}
		}

		std::vector<DS_LIGHT*> getNearbyLights(DS_NODE* ob) {
			std::vector<std::vector<DS_LIGHT*>> unflat_list; std::vector<DS_LIGHT*> obj_list;

			NOVELL* neighbor = nullptr;
			NOVELL* nov = this->getNov(ob->worldPosition());
			if (!nov) {
				//TODO: object is out of bounds -- queue deletion
				return obj_list;
			}

			ENNEAD* enn = nov->getEnn(ob->worldPosition());
			ENNEAD* n;

			unflat_list.push_back(nov->getOcc_l());
			for (unsigned int i = 0; i < enn->numNeighbors(); i++) {
				n = enn->getNeighbor(i);
				if (n) {
					if (n->isNeighborOuter(i)) {
						neighbor = this->novs[n->px][n->pz];
						std::vector<std::vector<DS_LIGHT*>>::iterator pp =
							std::find(unflat_list.begin(), unflat_list.end(), neighbor->getOcc_l());

						if (pp == unflat_list.end()) {
							unflat_list.push_back(neighbor->getOcc_l());
						}
					}
				}
			}

			for (unsigned int i = 0; i < unflat_list.size(); i++) {
				for (unsigned int j = 0; j < unflat_list[i].size(); j++) {
					obj_list.push_back(unflat_list[i][j]);
				}
			}

			return obj_list;

		}

		NOVELL* getNov(glm::vec3 pos) {
			int novi[2] = { (int)roundf(pos.x / 24) + 2, (int)roundf(pos.z / 24) + 2 };
			if (novi[0] > 4 || novi[1] > 4 || novi[0] < 0 || novi[1] < 0) { return nullptr; }
			return this->novs[novi[0]][novi[1]];
		}

	private:
		std::vector<std::vector<NOVELL*>> novs;

};

extern CELLTREE* ct;

#endif //SPACE_H
