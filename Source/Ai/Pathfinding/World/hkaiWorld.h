/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef HK_AI_WORLD_H
#define HK_AI_WORLD_H

#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
#include <Ai/Pathfinding/NavMesh/Streaming/hkaiStreamingCollection.h>
#include <Ai/Pathfinding/Dynamic/NavMeshCutter/hkaiNavMeshCutter.h>


#include <Ai/Pathfinding/Utilities/hkaiPathfindingUtil.h>
#include <Ai/Pathfinding/Utilities/hkaiVolumePathfindingUtil.h>
#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteGenerator/hkaiSilhouetteGenerationParameters.h>

class hkaiNavMeshGeneratorPartitioner;
class hkaiOverlapManager;
struct hkaiOverlapManagerSection;
class hkaiReferenceFrameAndExtrusion;
class hkaiSilhouetteGenerator;
class hkaiDynamicNavMeshQueryMediator;
class hkaiObstacleGenerator;
class hkaiPathRequestInfoOwner;
class hkaiDirectedGraphExplicitCost;
class hkaiNavMeshQueryMediator;
class hkaiNavVolume;
class hkaiNavVolumeMediator;
class hkaiCharacter;
struct hkaiLocalSteeringInput;
struct hkaiAvoidancePairProperties;

// Multithreading
class hkJobQueue;
class hkThreadPool;

extern const class hkClass hkaiWorldClass;

/// A single requested path in the hkaiWorld.
class hkaiNavMeshPathRequestInfo : public hkReferencedObject
{
	friend class hkaiWorld;

public:

	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

	inline hkaiNavMeshPathRequestInfo();
	inline hkaiNavMeshPathRequestInfo( hkFinishLoadedObjectFlag f );

	/// Get the current priority of this request.
	inline int getPriority() const;

	/// Has this path request been processed yet?
	inline hkBool32 isProcessed() const;

	//
	// Input and output accessors.
	//

	/// Access the input as a hkaiPathfindingUtil::FindPathInput*.
	inline hkaiPathfindingUtil::FindPathInput* getNavMeshInput();

	/// Access the input as a hkaiPathfindingUtil::FindPathInput*.
	inline const hkaiPathfindingUtil::FindPathInput* getNavMeshInput() const;

	/// Access the output as a hkaiPathfindingUtil::FindPathOutput*.
	inline hkaiPathfindingUtil::FindPathOutput* getNavMeshOutput();

	/// Access the output as a hkaiPathfindingUtil::FindPathOutput*.
	inline const hkaiPathfindingUtil::FindPathOutput* getNavMeshOutput() const;

	/// Schedule the request to be deleted during the next world step.
	inline void markForDelete();

	/// Whether or not the request is scheduled to be deleted
	inline bool isMarkedForDeletion() const;

	/// Delete the input and output structures (if they're non-null).
	inline void deallocate();

	/// Internal determinism check.
	void checkDeterminism();

	/// Sorting predicate for path requests.
	inline hkBool32 operator<(const hkaiNavMeshPathRequestInfo& other) const;

protected:
	hkRefPtr<hkaiPathfindingUtil::FindPathInput> m_input;
	hkRefPtr<hkaiPathfindingUtil::FindPathOutput> m_output;

	int m_priority;
	hkaiPathRequestInfoOwner* m_owner; //+nosave
	hkBool m_markedForDeletion;
};

/// A single requested path in the hkaiWorld.
class hkaiNavVolumePathRequestInfo : public hkReferencedObject
{
	friend class hkaiWorld;

public:
	HK_DECLARE_REFLECTION();
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

	inline hkaiNavVolumePathRequestInfo();
	inline hkaiNavVolumePathRequestInfo( hkFinishLoadedObjectFlag f );

	/// Get the current priority of this request.
	inline int getPriority() const;

	/// Has this path request been processed yet?
	inline hkBool32 isProcessed() const;

	//
	// Input and output accessors.
	//

	/// Access the input as a hkaiVolumePathfindingUtil::FindPathInput*.
	inline hkaiVolumePathfindingUtil::FindPathInput* getNavVolumeInput();

	/// Access the input as a hkaiVolumePathfindingUtil::FindPathInput*.
	inline const hkaiVolumePathfindingUtil::FindPathInput* getNavVolumeInput() const;

	/// Access the output as a hkaiVolumePathfindingUtil::FindPathOutput*.
	inline hkaiVolumePathfindingUtil::FindPathOutput* getNavVolumeOutput();

	/// Access the output as a hkaiVolumePathfindingUtil::FindPathOutput*.
	inline const hkaiVolumePathfindingUtil::FindPathOutput* getNavVolumeOutput() const;

	/// Schedule the request to be deleted during the next world step.
	inline void markForDelete();

	/// Whether or not the request is scheduled to be deleted
	inline bool isMarkedForDeletion() const;

	/// Delete the input and output structures (if they're non-null).
	inline void deallocate();

	/// Internal determinism check.
	void checkDeterminism();

	/// Sorting predicate for path requests.
	inline hkBool32 operator<(const hkaiNavVolumePathRequestInfo& other) const;

protected:
	hkRefPtr<hkaiVolumePathfindingUtil::FindPathInput> m_input;
	hkRefPtr<hkaiVolumePathfindingUtil::FindPathOutput> m_output;

	int m_priority;
	hkaiPathRequestInfoOwner* m_owner; //+nosave
	hkBool m_markedForDeletion;
};

/// This is a container for characters and objects added to the AI world.
class hkaiWorld : public hkReferencedObject
{
	// +version(31)
	public:

		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

			/// The construction information for a hkaiWorld.
		struct Cinfo
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE,hkaiWorld::Cinfo);
			Cinfo();

				/// The up vector. This should be consistent for all characters / nav meshes / generators in the world
			hkVector4	m_up;

				/// Clearance reset method used by hkaiNavMeshCutter.
			hkEnum<hkaiNavMeshCutter::ClearanceResetMethod, hkUint8> m_clearanceResetMethod; //+default(hkaiNavCutter::CLEARANCE_RESET_ALL)

				/// Whether or not to do nav mesh validation during cutting.
				/// These are always disabled during release builds, but enabled by default in debug builds.
				/// The checks are potentially slow, but should be left on initially, 
				/// at least until streaming and cutting are verified to be in your engine.
			hkBool	m_performValidationChecks; //+default(true)

				/// Vertical expansion for silhouette
			hkReal m_silhouetteExtrusion; //+default(1.5f)

				/// Bevel threshold for silhouette generation.
			hkReal m_bevelThreshold; //+default(.4f)

				/// Maximum silhouette extents that is guaranteed to be supported.
				/// The domain used for triangulation is the hkaiNavMesh::Face's AABB expanded by this value.
				/// Increasing this may lead to loss of accuracy for faces created by nav mesh cutting.
			hkReal m_maxSilhouetteSize; //+default(50.0f)

				/// Area-based silhouette simplification threshold
			hkReal m_simplify2dConvexHullThreshold; //+default(.005f);

				/// Maximum number of faces that are cut each step.
				/// If this value is negative, then no limits are enforced.
			int m_maxCutFacesPerStep; //+default(-1)

				
				/// The world avoidance properties shared by all character pairs
			struct hkaiAvoidancePairProperties*	m_avoidancePairProps;

				/// Maximum number of requests per step. This is a "soft" maximum - requests above m_priorityThreshold are always processed.
			int m_maxRequestsPerStep; //+default(100)

			
				/// Estimated limit of A* searches per step. This is not a true hard limit, as the limit cannot be 
				/// arbitrated between multiple jobs executing in parallel. This iteration limit will be distributed
				/// across all submitted jobs, according to the jobs' estimated costs.
			int m_maxEstimatedIterationsPerStep; //+default(-1)
			

				/// Requests with priority greater or equal to this value will be processed regardless of m_maxRequestsPerStep.
			int m_priorityThreshold; //+default(1024)

			//
			// Multithreading parameters
			// These control the granularity of the multithreaded jobs; setting them too low will cause
			// contention between threads.
			//

				/// Number of path requests to process per hkaiNavMeshAStarJob.
			int m_numPathRequestsPerJob; //+default(16)

				/// Number of character behaviors to update per hkaiBehaviorCalcVelocitiesJob.
			int m_numBehaviorUpdatesPerJob; //+default(16)

				/// Number of characters to solver per hkaiAvoidanceSolverJob.
			int m_numCharactersPerAvoidanceJob; //+default(16)

				/// Maximum number of edges that can be output during multithreaded character searches.
				/// See hkaiNavMeshAstarCommand::m_maxEdgesOut
			int m_maxPathSearchEdgesOut; //+default(64)

				/// Maximum number of path points that can be output during multithreaded character searches.
				/// See hkaiNavMeshAstarCommand::m_maxPathSizeOut
			int m_maxPathSearchPointsOut; // +default(32)

				/// Pathfinding input options to use when generating requests.
			hkaiPathfindingUtil::FindPathInput m_pathfindingInput;

				/// Pathfinding input options to use when generating requests.
			hkaiVolumePathfindingUtil::FindPathInput m_volumePathfindingInput;

		};

			// Construct a world using hkaiWorld::Cinfo
		hkaiWorld( const Cinfo& cInfo );

			/// Serialization constructor
		hkaiWorld( hkFinishLoadedObjectFlag f );

		virtual ~hkaiWorld();


		//
		// Callback context
		//

		enum StepThreading
		{
			STEP_SINGLE_THREADED,
			STEP_MULTI_THREADED,
		};

			/// Information about the silhouette step that is passed to	hkaiWorld::Listener::dynamicNavMeshModifiedCallback().
		struct NavMeshModifiedCallbackContext
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI,hkaiWorld::NavMeshModifiedCallbackContext);
			NavMeshModifiedCallbackContext( class hkaiWorld* world, const hkArrayBase<hkaiPackedKey>& cutFaceKeys, const hkArrayBase<hkaiPackedKey>& uncutFaceKeys );

			class hkaiWorld* m_world;
			const hkArrayBase<hkaiPackedKey>& m_cutFaceKeys;
			const hkArrayBase<hkaiPackedKey>& m_uncutFaceKeys;
		};

		enum CharacterCallbackType
		{
			CALLBACK_PRECHARACTER_STEP,
			CALLBACK_POSTCHARACTER_STEP
		};

			/// Information about the characters in the world that is passed to
			/// hkaiWorld::Listener::preCharacterStepCallback() and postCharacterStepCallback().
		struct CharacterStepCallbackContext
		{
		public:
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, CharacterStepCallbackContext);

			CharacterStepCallbackContext( CharacterCallbackType cbType, class hkaiWorld* world, hkReal timestep, const hkArrayBase<hkaiCharacter*>* characters, const hkArrayBase<hkaiLocalSteeringInput>* localSteeringInputs );
			~CharacterStepCallbackContext() {}

			class hkaiWorld* m_world; 
			hkEnum<CharacterCallbackType, hkUint8> m_callbackType;
			hkReal m_timestep;
			const hkArrayBase<hkaiCharacter*>* m_characters; 
			const hkArrayBase<hkaiLocalSteeringInput>* m_localSteeringInputs; 
		};

			/// A serializable version of CharacterStepCallbackContext. This adds temporary reference counts to the
			/// characters, so should be avoided unless needed.
		class CharacterStepSerializableContext : public hkReferencedObject
		{
		public:
			//+version(2)
			HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
			HK_DECLARE_REFLECTION();

			CharacterStepSerializableContext( const CharacterStepCallbackContext& cbCtx );
			CharacterStepSerializableContext( hkFinishLoadedObjectFlag f );
			~CharacterStepSerializableContext();

			class hkaiWorld* m_world; //+nosave
			hkEnum<CharacterCallbackType, hkUint8> m_callbackType;
			hkReal m_timestep; //+default(1.0f / 30.0f);
			hkArray< hkRefPtr<const hkaiCharacter> > m_characters; 
			const hkArray<struct hkaiLocalSteeringInput> m_localSteeringInputs; 
			hkArray<hkRefPtr<hkaiObstacleGenerator> > m_obstacleGenerators;
		};


		//
		// Listeners
		//

			/// hkaiWorld callback interface.
		class Listener
		{
			public:
				HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI,hkaiWorld::Listener);
				virtual ~Listener() {}

					/// raised before characters are integrated for local steering.
				virtual void preCharacterStepCallback( const CharacterStepCallbackContext& ctx ) {}

					/// raised after characters are integrated for local steering.
				virtual void postCharacterStepCallback( const CharacterStepCallbackContext& ctx ) {}

					/// raised after hkaiWorld::step is called.
				virtual void postStepCallback(class hkaiWorld* world, const hkArrayBase<class hkaiBehavior*>& behaviors) {}

					/// raised before silhouettes are stepped and cut
				virtual void preSilhouetteStepCallback( const class hkaiWorld* world, StepThreading threading, const class hkBitField* sectionsToStep ) {}

					/// raised when the dynamic nav mesh is changed.
				virtual void dynamicNavMeshModifiedCallback( NavMeshModifiedCallbackContext& context ) {}

					/// Raised for each nav mesh path request that is processed.
				virtual void pathRequestProcessedCallback( const hkaiNavMeshPathRequestInfo& request ) {}

					/// Raised for each nav volume path request that is processed
				virtual void pathRequestProcessedCallback( const hkaiNavVolumePathRequestInfo& request ) {}

					/// Raised when a nav mesh instance is added to the world.
				virtual void navMeshInstanceAdded( const class hkaiWorld* world, hkaiNavMeshInstance* navMeshInstance, const hkaiNavMeshQueryMediator* mediator, hkaiDirectedGraphInstance* hierarchyGraph ) {}

					/// Raised when a nav mesh instance is removed from the world.
				virtual void navMeshInstanceRemoved( const class hkaiWorld* world, hkaiNavMeshInstance* navMeshInstance, hkaiDirectedGraphInstance* hierarchyGraph  ) {}

					/// Raised when a nav volume is added to the world.
				virtual void navVolumeAdded( const class hkaiWorld* world, const class hkaiNavVolumeInstance* volume, const class hkaiNavVolumeMediator* mediator ) {}

					/// Raised when a nav volume is removed from the world.
				virtual void navVolumeRemoved( const class hkaiWorld* world, const class hkaiNavVolumeInstance* volume ) {}

					/// Raised when a directed graph is added to the world. This is NOT called when a nav mesh instance along with the directed graph is loaded.
				virtual void graphAdded( const class hkaiWorld* world, const class hkaiDirectedGraphInstance* graph ) {}

					/// Raised when a directed graph is removed from the world. This is NOT called when a nav mesh instance along with the directed graph is unloaded.
				virtual void graphRemoved( const class hkaiWorld* world, const class hkaiDirectedGraphInstance* graph ) {}

					/// Raised when the world's destructor is called
				virtual void worldDeletedCallback( hkaiWorld* world ) {}

		};

			/// Adds a listener. The most common type of listener is a viewer for debug data
		void addListener( class hkaiWorld::Listener* listener );

			/// Removes a listener
		void removeListener( class hkaiWorld::Listener* listener );

		//
		// Nav mesh and streaming
		//

			/// This sets the nav mesh associated with this AI world. The mesh is dynamically cut by any generators added to the world.
			/// An hkaiNavMeshQueryMediator must also be provided, in order to manage intersections between the nav mesh and generators.
			/// Optionally, hierarchy information (e.g., computed from hkaiHierarchyUtils::clusterNavMesh) can be specified too.
			/// This method should only be used if ONE mesh will be in the AI world at a time. For multiple meshes, use loadNavMeshInstance/unloadNavMeshInstance.
		void setNavMesh( const class hkaiNavMesh* navMesh, hkaiNavMeshQueryMediator* mediator, const hkaiDirectedGraphExplicitCost* hierarchyGraph = HK_NULL, bool setupClearanceInfo = true);

		hkaiNavMeshQueryMediator* getDynamicQueryMediator();
		const hkaiNavMeshQueryMediator* getDynamicQueryMediator() const;

		const hkaiNavVolumeMediator* getDynamicNavVolumeMediator() const;

		/// Add a nav mesh instance to the world, and connect edges between other meshes
		void loadNavMeshInstance( hkaiNavMeshInstance* navMeshInstance, const hkaiNavMeshQueryMediator* mediator, hkaiDirectedGraphInstance* hierachyGraphInstance = HK_NULL);

		/// Remove a nav mesh instance from the world, and disconnect edges between other meshes
		void unloadNavMeshInstance( hkaiNavMeshInstance* navMeshInstance, hkaiDirectedGraphInstance* hierachyGraphInstance );

		/// Add a nav mesh cluster graph to the world (without its corresponding nav mesh), and connect edges between other graphs
		void loadNavMeshGraph( hkaiDirectedGraphInstance* hierarchyGraph );

		/// Remove a nav mesh cluster graph from the world, and disconnect edges between other graphs
		void unloadNavMeshGraph( hkaiDirectedGraphInstance* hierarchyGraph );

		/// Add a nav volume to the world, and connect edges between other volume
		void loadNavVolume( hkaiNavVolumeInstance* navVolume, hkaiNavVolumeMediator* mediator);

		/// Remove a nav mesh from the world, and disconnect edges between other volumes
		void unloadNavVolume( hkaiNavVolumeInstance* navVolume );

			/// Access the world's streaming collection
		inline hkaiStreamingCollection* getStreamingCollection();

			/// Read-only access the world's streaming manager
		inline const hkaiStreamingCollection* getStreamingCollection() const;

			/// Access to the world's nav mesh cutter
		inline hkaiNavMeshCutter* getNavMeshCutter();

			/// Read-only access to the world's nav mesh cutter
		inline const hkaiNavMeshCutter* getNavMeshCutter() const;

		//
		// Silhouette obstacles
		//

		typedef hkArray< hkRefPtr<hkaiSilhouetteGenerator> > SilhouetteGeneratorArray;

			/// Add a generator to the world. The generator will be used to update the dynamic nav mesh.
		void addSilhouetteGenerator( class hkaiSilhouetteGenerator* generator );

			/// Removes a generator from the world.
		void removeSilhouetteGenerator( class hkaiSilhouetteGenerator* generator );

			/// Modify the silhouette extrusion on the fly (and force recomputation of all silhouettes)
		void setSilhouetteExtrusion( hkReal e );

			/// Get whether to force silhouette overlap updates every frame
		inline bool getForceSilhouetteUpdates() const;

			/// Set whether to force silhouette overlap updates every frame.
			/// This is useful for profiling, but should not be used for real builds
		inline void setForceSilhouetteUpdates( bool forceSilhouetteUpdates );

			/// Get all the silhouette generators for the world. You should NOT add or remove silhouettes by modifying this array.
		inline hkaiWorld::SilhouetteGeneratorArray&	getSilhouetteGenerators(); 

		inline const hkaiWorld::SilhouetteGeneratorArray&	getSilhouetteGenerators() const; 

			/// Access to the world's overlap manager
		inline hkaiOverlapManager* getOverlapManager();

			/// Access to the world's overlap manager
		inline const hkaiOverlapManager* getOverlapManager() const;

			/// Access to the world's silhouette generation parameters.
		inline hkaiSilhouetteGenerationParameters& getSilhouetteGenerationParams();
			
			/// Access to the world's silhouette generation parameters.
		inline const hkaiSilhouetteGenerationParameters& getSilhouetteGenerationParams() const;

			/// Resets all silhouettes in the world.
			/// This should generally NOT be called by the user as it may be expensive. It is primarily for testing and debugging.
		void invalidateAllSilhouettes();

			/// Remove the existing silhouettes and overlap manager and apply the new ones
			/// This should only be used when "replaying" silhouettes for debugging.
		void forceSilhouetteInformation( const hkArray< hkRefPtr<hkaiSilhouetteGenerator> >& silhouettes, hkArray<hkaiOverlapManagerSection>& managerSections );

		//
		// Local steering obstacles and properties
		//

		typedef hkArray< hkRefPtr<hkaiObstacleGenerator> > ObstacleGeneratorArray;

		/// Add an obstacle generator to the world. These obstacle are avoided by characters during local steering.
		void addObstacleGenerator(hkaiObstacleGenerator* generator);

			/// Remove an obstacle generator.
		void removeObstacleGenerator(hkaiObstacleGenerator* generator);

			/// Get the array of obstacle generators. You should NOT add or remove generators by modifying this array.
		inline ObstacleGeneratorArray& getObstacleGenerators();

			/// Get the array of obstacle generators.
		inline const ObstacleGeneratorArray& getObstacleGenerators() const;

			
			/// Access to pair-wise avoidance properties.
		inline const hkaiAvoidancePairProperties* getAvoidancePairProperties() const;
			
			/// Sets the pair-wise avoidance properties
		void setAvoidancePairProperties( hkaiAvoidancePairProperties* props);

			/// Number of character behaviors to update per hkaiBehaviorCalcVelocitiesJob.
		inline int getNumBehaviorUpdatesPerJob() const;

			/// Set then number of character behaviors to update per hkaiBehaviorCalcVelocitiesJob.
		inline void setNumBehaviorUpdatesPerJob(int n);

	

		//
		// Character pathfinding
		//

			/// Request a path.
		void requestPath(hkaiPathRequestInfoOwner* owner, hkaiPathfindingUtil::FindPathInput* input, int priority = 0);

		/// Request a path.
		void requestPath(hkaiPathRequestInfoOwner* owner, hkaiVolumePathfindingUtil::FindPathInput* input, int priority = 0);

			/// Returns the A* request for the given owner, if found, or HK_NULL otherwise.
		const hkaiNavMeshPathRequestInfo* getNavMeshPathRequest(const hkaiPathRequestInfoOwner* owner) const;

			/// Returns the A* request for the given owner, if found, or HK_NULL otherwise.
		const hkaiNavVolumePathRequestInfo* getNavVolumePathRequest(const hkaiPathRequestInfoOwner* owner) const;

			/// Returns the A* request for the given owner, if found, or HK_NULL otherwise.
		hkaiNavMeshPathRequestInfo* getNavMeshPathRequest(const hkaiPathRequestInfoOwner* owner);

		/// Returns the A* request for the given owner, if found, or HK_NULL otherwise.
		hkaiNavVolumePathRequestInfo* getNavVolumePathRequest(const hkaiPathRequestInfoOwner* owner);

			/// Cancel the A* request for the given owner and return it
		const hkaiNavMeshPathRequestInfo* cancelRequestedNavMeshPath(const hkaiPathRequestInfoOwner* owner);

			/// Cancel the A* request for the given owner and return it
		const hkaiNavVolumePathRequestInfo* cancelRequestedNavVolumePath(const hkaiPathRequestInfoOwner* owner);


			/// Increases the priority of all existing path requests by 1. Called at the end of stepCharacters and stepCharactersMT.
		void incrementRequestPriorities();

			/// Default values for pathfinding
		const hkaiPathfindingUtil::FindPathInput& getDefaultPathfindingInput() const;

			/// Default values for pathfinding
		hkaiPathfindingUtil::FindPathInput& getDefaultPathfindingInput();

		/// Default values for pathfinding
		const hkaiVolumePathfindingUtil::FindPathInput& getDefaultVolumePathfindingInput() const;

		/// Default values for pathfinding
		hkaiVolumePathfindingUtil::FindPathInput& getDefaultVolumePathfindingInput();
		
			/// Maximum number of requests per step. This is a "soft" maximum - requests above m_priorityThreshold are always processed.
		inline int getMaxRequestsPerStep() const;

			/// Set maximum number of requests per step.
		inline void setMaxRequestsPerStep( int m );

			/// Estimated limit of A* searches per step. This is not a true hard limit, as the limit cannot be 
			/// arbitrated between multiple jobs executing in parallel. This iteration limit will be distributed
			/// across all submitted jobs, according to the jobs' estimated costs.
		inline int getMaxEstimatedIterationsPerStep() const;

			/// Set maximum estimated iterations per step.
		inline void setMaxEstimatedIterationsPerStep( int maxEstimatedIterationsPerStep );

			/// Requests with priority greater or equal to this value will be processed regardless of m_maxRequestsPerStep.
		inline int getPriorityThreshold() const;

			/// Requests with path search priority threshold.
		inline void setPriorityThreshold( int t );

			/// Number of path requests to process per hkaiNavMeshAStarJob. Only used during stepPathSearchesMT()
		inline int getNumPathRequestsPerJob() const;

			/// Set the number of path requests to process per hkaiNavMeshAStarJob.
		inline void setNumPathRequestsPerJob(int n);

			/// Number of characters to solver per hkaiAvoidanceSolverJob.
		inline int getNumCharactersPerAvoidanceJob() const;

			/// Set the number of characters to solver per hkaiAvoidanceSolverJob.
		inline void setNumCharactersPerAvoidanceJob(int n) ;

			/// Maximum number of edges that can be output during character searches.
		inline int getMaxPathSearchEdgesOut() const;

			/// Set the maximum number of edges that can be output during character searches.
		inline void setMaxPathSearchEdgesOut(int n);

			/// Maximum number of path points that can be output during character searches.
		inline int getMaxPathSearchPointsOut() const;

			/// Set the maximum number of path points that can be output during character searches.
		inline void setMaxPathSearchPointsOut( int n );

			/// Get the erosion radius
		inline hkReal getErosionRadius() const;

			/// Get the erosion radius - all subsequent nav meshes added should have the same erosion radius.
		inline void setErosionRadius(hkReal erosionRadius);

	protected:
			/// Deletes the specified request (by order in the array)
		void _deleteNavMeshPathRequestAt( int i );
		void _deleteNavVolumePathRequestAt( int i );

	public:

		//
		// Update/stepping functions
		//

			/// Simple wrapper for stepping
		void step(hkReal timestep, hkArrayBase<hkaiBehavior*>& behaviors);

			/// Simple wrapper for multithreaded stepping
		void stepMultithreaded(hkReal timestep, hkArrayBase<hkaiBehavior*>& behaviors, hkJobQueue* jobQueue, hkJobThreadPool* threadPool);

		//
		// Advanced use only: individual stepping functions. These are called by the "simple" wrappers.
		//

			/// Update the nav mesh based on the silhouettes.
		void stepSilhouettes( const class hkBitField* sectionsToStep = HK_NULL );

			/// Update path requests.
		void stepPathSearches();

			/// Character local steering.
		void stepCharacters( hkReal timestep, hkArrayBase<hkaiCharacter*>& characters, hkArrayBase<class hkaiBehavior*>& behaviors );


			/// Update the nav mesh based on the silhouettes, using multithreaded jobs.
		void stepSilhouettesMT( const class hkBitField* sectionsToStep, hkJobQueue* jobQueue, hkJobThreadPool* threadPool );

			/// Update path requests, using multithreaded jobs.
		void stepPathSearchesMT(hkJobQueue* jobQueue, hkJobThreadPool* threadPool );

			/// Character local steering, using multithreaded jobs.
		void stepCharactersMT( hkReal timestep, hkArrayBase<hkaiCharacter*>& characters, hkArrayBase<class hkaiBehavior*>& behaviors, hkJobQueue* jobQueue, hkJobThreadPool* threadPool );
		
			/// Fires all the Listener's postStepCallbacks
		void firePostStepCallbacks(const hkArrayBase<hkaiBehavior*>& behaviors);

			/// Move the world by the specified amount.
			/// This will
			/// * Change the transform in each hkaiNavMeshInstances in the hkaiStreamingCollection 
			/// * Call hkaiSilhouetteGenerators::shiftWorldSpace() for each hkaiSilhouetteGenerator
			/// * Call hkaiObstacleGenerator::shiftWorldSpace() for each hkaiObstacleGenerator
			/// * Shift the start points, end points, and output paths of all queued path requests
			/// Note that hkaiCharacters and hkaiBehaviors are not affected; they must be adjusted manually.
			/// Any points stored in world space that are not owned by the world (e.g. hkaiUserEdgeUtils::UserEdgePair) are not affected.
		void shiftWorldSpace( hkVector4Parameter shift );

		enum PathType
		{
			/// Nav mesh
			PATH_TYPE_NAVMESH = 0,

			/// Nav volume
			PATH_TYPE_NAVVOLUME,
		};

			/// Temporary struct for sorting path requests by priority.
		struct SortedRequest
		{
			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, SortedRequest);

			void* m_request;
			PathType m_pathType;
			int m_priority;

			hkBool32 operator<(const SortedRequest& other) const { return m_priority > other.m_priority; }
		};
		
			/// Makes sure the character's current nav mesh face is up to date
		void updateMeshFaceForCharacter(hkaiCharacter& character);

protected:

			/// Called internally by shiftWorldSpace.
		void shiftPathRequests( hkVector4Parameter shift );

		void setupReferenceFrame( hkaiReferenceFrameAndExtrusion& refFrame );

			/// Deletes stale requests and sorts the request array by priority.
		void prepareRequestArray( hkArray<SortedRequest>& m_sortedRequests );

			/// Calls hkaiBehavior::calcVelocities on the array of hkaiBehaviors, fills in local steering input for each character
		void calcDesiredVelocities( hkReal timestep, int numCharacters, hkArrayBase<hkaiBehavior*>& behaviors, 
			hkJobQueue* jobQueue, hkJobThreadPool* threadPool, hkArray<hkaiLocalSteeringInput>::Temp& localSteeringInputs );

		friend class hkaiWorldViewer;

	public:
		hkVector4					m_up;

	protected:
			/// The hkaiStreamingCollection maintains all the static navmesh information and is used for A*
		hkRefPtr<hkaiStreamingCollection>	m_streamingCollection;

			/// Dynamic nav mesh system
		hkRefPtr<hkaiNavMeshCutter>		m_cutter;

			/// Whether or not to do nav mesh validation during cutting.
			/// These are always disabled during release builds, but potentially slow in debug builds
		hkBool	m_performValidationChecks; //+default(true)

		hkRefPtr<class hkaiDynamicNavMeshQueryMediator>	m_dynamicNavMeshMediator;
		hkRefPtr<class hkaiDynamicNavVolumeMediator>	m_dynamicNavVolumeMediator;

		hkRefPtr<hkaiOverlapManager>		m_overlapManager;
		hkaiSilhouetteGenerationParameters	m_silhouetteGenerationParameters;
		hkReal								m_silhouetteExtrusion;
		hkBool								m_forceSilhouetteUpdates; //+default(false)

		hkArray<hkaiWorld::Listener*>		m_listeners; //+nosave

			/// All silhouette generators
		SilhouetteGeneratorArray	m_silhouetteGenerators;

			/// All obstacle generators
		ObstacleGeneratorArray		m_obstacleGenerators;

		
		/// Pair-wise avoidance properties for local steering
		hkRefPtr<hkaiAvoidancePairProperties>	m_avoidancePairProps;

		hkArray<hkaiNavMeshPathRequestInfo> m_navMeshPathRequests; //+nosave

		hkArray<hkaiNavVolumePathRequestInfo> m_navVolumePathRequests; //+nosave

		
		
			/// While stepping paths, callbacks are not allowed to request a new path or cancel existing ones.
		hkBool m_isPathRequestArrayLocked; //+nosave

			/// Maximum number of requests per step. This is a "soft" maximum - requests above m_priorityThreshold are always processed.
		int m_maxRequestsPerStep;

			/// Estimated limit of A* searches per step. This is not a true hard limit, as the limit cannot be 
			/// arbitrated between multiple jobs executing in parallel. This iteration limit will be distributed
			/// across all submitted jobs, according to the jobs' estimated costs.
		int m_maxEstimatedIterationsPerStep;

			/// Requests with priority greater or equal to this value will be processed regardless of m_maxRequestsPerStep.
		int m_priorityThreshold;

			/// Number of path requests to process per hkaiNavMeshAStarJob.
		int m_numPathRequestsPerJob;

			/// Number of character behaviors to update per hkaiBehaviorCalcVelocitiesJob.
		int m_numBehaviorUpdatesPerJob;

			/// Number of characters to solver per hkaiAvoidanceSolverJob.
		int m_numCharactersPerAvoidanceJob;

			/// Maximum number of edges that can be output during character searches.
		int m_maxPathSearchEdgesOut; 

			/// Maximum number of path points that can be output during character searches.
		int m_maxPathSearchPointsOut; 

			/// Pathfinding input options to use when generating requests.
		hkaiPathfindingUtil::FindPathInput m_defaultPathfindingInput;

			/// Pathfinding input options to use when generating requests.
		hkaiVolumePathfindingUtil::FindPathInput m_defaultVolumePathfindingInput;

		hkReal m_erosionRadius;

};

#include <Ai/Pathfinding/World/hkaiWorld.inl>

#endif // HK_AI_WORLD_H

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
