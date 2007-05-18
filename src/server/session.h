/*******************************************************************************

   Copyright (C) 2006, 2007 M.K.A. <wyrmchild@users.sourceforge.net>
   For more info, see: http://drawpile.sourceforge.net/

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#ifndef ServerSession_INCLUDED
#define ServerSession_INCLUDED

#include "common.h"

#include "../shared/protocol.flags.h"
#include "../shared/protocol.tools.h"
#include "../shared/protocol.defaults.h"

#include "../shared/memstack.h"

struct User; // defined elsewhere
#ifndef ServerUser_INCLUDED
	#include "user.h"
#endif

#include <limits> // std::numeric_limits<T>

#if defined(HAVE_SLIST)
	#include <ext/slist>
#else
	#include <list>
#endif

struct LayerData
{
	LayerData() throw()
		: id(protocol::null_layer),
		mode(protocol::tool_mode::None),
		opacity(0),
		locked(false)
	{
	}
	
	LayerData(const uint _id, const uint _mode, const uint _opacity=std::numeric_limits<uint8_t>::max(), const bool _locked=false) throw()
		: id(_id),
		mode(_mode),
		opacity(_opacity),
		locked(_locked)
	{
	}
	
	~LayerData() throw()
	{
		
	}
	
	uint
		// identifier
		id,
		// composition mode
		mode,
		// transparency/opacity
		opacity;
	
	bool locked;
};

/* iterators */
#if defined(HAVE_HASH_MAP)
#include <ext/hash_map>
typedef __gnu_cxx::hash_map<uint8_t, User*>::iterator session_usr_i;
typedef __gnu_cxx::hash_map<uint8_t, User*>::const_iterator session_usr_const_i;
typedef __gnu_cxx::hash_map<uint8_t, LayerData>::iterator session_layer_i;
typedef __gnu_cxx::hash_map<uint8_t, LayerData>::const_iterator session_layer_const_i;
#else
#include <map>
typedef std::map<uint8_t, User*>::iterator session_usr_i;
typedef std::map<uint8_t, User*>::const_iterator session_usr_const_i;
typedef std::map<uint8_t, LayerData>::iterator session_layer_i;
typedef std::map<uint8_t, LayerData>::const_iterator session_layer_const_i;
#endif

// Session information
struct Session
	//: MemoryStack<Session>
{
	Session(const uint _id, uint _mode, uint _limit, uint _owner,
		uint _width, uint _height, uint _level, uint _title_len, char* _title) throw()
		: id(_id),
		title_len(_title_len),
		title(_title),
		pw_len(0),
		password(0),
		mode(_mode),
		limit(_limit),
		owner(_owner),
		width(_width),
		height(_height),
		level(_level),
		SelfDestruct(true),
		syncCounter(0),
		locked(false)
	{
		#ifndef NDEBUG
		std::cout << "Session::Session(ID: " << static_cast<int>(id) << ")" << std::endl;
		#endif
	}
	
	~Session() throw()
	{
		#ifndef NDEBUG
		std::cout << "Session::~Session(ID: " << static_cast<int>(id) << ")" << std::endl;
		#endif
		
		delete [] title;
	}
	
	// Session identifier
	uint id;
	
	// Title length
	uint title_len;
	
	// Session title
	char* title;
	
	// Password length
	uint pw_len;
	
	// Password string
	char* password;
	
	// Default user mode
	uint8_t mode;
	
	// User limit
	uint limit;
	
	// Session owner
	uint owner;
	
	// Canvas size
	uint width, height;
	
	// Feature level required
	uint level;
	
	// Will the session be destructed when all users leave..?
	bool SelfDestruct;
	
	uint8_t getFlags() const throw()
	{
		return (SelfDestruct?0:protocol::session::NoSelfDestruct);
	}
	
	// 
	#if defined(HAVE_HASH_MAP)
	__gnu_cxx::hash_map<uint8_t, LayerData> layers;
	//__gnu_cxx::hash_set<LayerData> layers;
	#else
	std::map<uint8_t, LayerData> layers;
	//std::set<LayerData> layers;
	#endif
	
	// Subscribed users
	#if defined(HAVE_HASH_MAP)
	__gnu_cxx::hash_map<uint8_t, User*> users;
	#else
	std::map<uint8_t, User*> users;
	#endif
	
	// Users waiting sync.
	#if defined(HAVE_SLIST)
	__gnu_cxx::slist<User*> waitingSync;
	#else // 
	std::list<User*> waitingSync;
	#endif
	
	// Session sync in action.
	uint syncCounter;
	
	// Session is locked, preventing any drawing to take place.
	bool locked;
	
	/* *** Functions *** */
	
	// Session can be joined
	bool canJoin() const throw()
	{
		return ((users.size() + waitingSync.size()) < limit);
	}
};

#endif // ServerSession_INCLUDED
