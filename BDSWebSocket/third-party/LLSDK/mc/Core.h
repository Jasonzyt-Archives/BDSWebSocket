#pragma once
//#include<lbpch.h>
#define MCAPI __declspec(dllimport)
#define MCINLINE inline
#define MCCLS
#include"mass.h"
#include <string>
//#include<stl/Bstream.h>
enum ActorType : int;
enum class AbilitiesIndex : int;
static inline int iround(float x) {
	int r = int(x);
	if (x < 0)
		r--;
	return r;
}

class Vec3 {
public:
	float x, y, z;
	std::string toString() {
		return "(" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + ")";
	}
	bool operator==(const Vec3& _Right)
	{
		if (x == _Right.x && y == _Right.y && z == _Right.z) 
			return true;
		return false;
	}
	//template<typename _TP>
/*	void pack(WBStreamImpl<_TP>& ws) const {
		ws.apply(x, y, z);
	}
	void unpack(RBStream& rs) {
		rs.apply(x, y, z);
	}*/
};

class BlockPos {
public:
	int x, y, z;
	inline bool operator==(BlockPos const& rv)const {
		return x == rv.x && y == rv.y && z == rv.z;
	}
	inline bool operator!=(BlockPos const& rv)const {
		return x != rv.x || y != rv.y || z != rv.z;
	}
	inline std::string toString() {
		return std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z);
	}
	//template<typename _TP>
	/*void pack(WBStreamImpl<_TP>& ws) const {
		ws.apply(x, y, z);
	}
	void unpack(RBStream& rs) {
		rs.apply(x, y, z);
	}*/
};

struct IVec2 {
	int x, z;
	IVec2(Vec3 l) {
		x = iround(l.x);
		z = iround(l.z);
	}
	IVec2(int a,int b):x(a),z(b){}
	void operator+=(int v) {
		x += v;
		z += v;
	}
};
template <typename A, typename T>
class AutomaticID {
	T id;
public:
	AutomaticID() {
		id = 0;
	}
	AutomaticID(T x) {
		id = x;
	}
	operator T() {
		return id;
	}
};
struct Tick {
	unsigned long long t;
};
struct ActorUniqueID {
	long long id;
public:
	ActorUniqueID() {
		id = -1;
	}
	ActorUniqueID(long long i) {
		id = i;
	}
	auto get() {
		return id;
	}
	operator long long() {
		return id;
	}
};
static_assert(!std::is_pod_v<ActorUniqueID>);
class ActorRuntimeID {
public:
	unsigned long long id;
	auto get() {
		return id;
	}
	operator unsigned long long() {
		return id;
	}
};
static_assert(std::is_pod_v<ActorRuntimeID>);
class ActorDamageSource {
private:
	char filler[0x10];

public:
	/*0*/ virtual ~ActorDamageSource();
	/*1*/ virtual bool isEntitySource() const;
	/*2*/ virtual void __unk_vfn_0();
	/*3*/ virtual bool isBlockSource() const;
	/*4*/ virtual bool isFire() const;
	/*5*/ virtual struct std::pair<std::string, std::vector<std::string> > getDeathMessage(std::string, class Actor*) const;
	/*6*/ virtual bool getIsCreative() const;
	/*7*/ virtual bool getIsWorldBuilder() const;
	/*8*/ virtual void __unk_vfn_1();
	/*9*/ virtual void __unk_vfn_2();
	/*10*/ virtual int /*enum enum ActorCategory*/ getEntityCategories() const;
	/*11*/ virtual bool getDamagingEntityIsCreative() const;
	/*12*/ virtual bool getDamagingEntityIsWorldBuilder() const;
	/*13*/ virtual struct ActorUniqueID getDamagingEntityUniqueID() const;
	/*14*/ virtual int /*enum enum ActorType*/ getDamagingEntityType() const;
	/*15*/ virtual int /*enum enum ActorCategory*/ getDamagingEntityCategories() const;
	/*16*/ virtual std::unique_ptr<class ActorDamageSource> clone() const;
};

class ChunkPos {
public:
	int x, z;
};

constexpr const int SAFE_PADDING = 0;