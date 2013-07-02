/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKAI_SILHOUETTE_REPLAY_VIEWER_H
#define HKAI_SILHOUETTE_REPLAY_VIEWER_H

#include <Common/Visualize/hkProcess.h>
#include <Ai/Visualize/VisualDebugger/Viewer/hkaiViewerBase.h>
#include <Ai/Pathfinding/World/hkaiWorld.h>

#include <Ai/Pathfinding/Dynamic/Silhouette/SilhouetteGenerator/hkaiSilhouetteGenerator.h>
#include <Ai/Pathfinding/Dynamic/Partitioning/hkaiOverlapManager.h>

class hkProcessContext;
extern const class hkClass hkaiSilhouetteRecorderReplayEventClass;
extern const class hkClass hkaiSilhouetteRecorderWorldConnectedEventClass;
extern const class hkClass hkaiSilhouetteRecorderInstanceLoadedEventClass;
extern const class hkClass hkaiSilhouetteRecorderSilhouettesSteppedEventClass;
extern const class hkClass hkaiSilhouetteRecorderInstanceUnloadedEventClass;
extern const class hkClass hkaiSilhouetteRecorderVolumeLoadedEventClass;
extern const class hkClass hkaiSilhouetteRecorderVolumeUnloadedEventClass;
extern const class hkClass hkaiSilhouetteRecorderGraphLoadedEventClass;
extern const class hkClass hkaiSilhouetteRecorderGraphUnloadedEventClass;


	/// General interface for creating and sending replay events
class hkaiSilhouetteRecorder : public hkaiWorld::Listener
{
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI, hkaiSilhouetteRecorder);

		/// Base replay event, only contains the type so that it can be cast to the appropriate subclass.
	class ReplayEvent : public hkReferencedObject
	{
	public:
		enum ReplayEventType
		{
				/// Triggered when a world is first connected with the viewer.
			EVENT_CONNECT_TO_WORLD,

				/// Triggered when an hkaiNavMeshInstance is loaded.
			EVENT_INSTANCE_LOADED,

				/// Triggered when an hkaiNavMeshInstance is unloaded.
			EVENT_INSTANCE_UNLOADED,

				/// Triggered before silhouettes are stepped by the hkaiWorld.
			EVENT_STEP_SILHOUETTES,

				/// Triggered when an hkaiNavVolume is loaded.
			EVENT_VOLUME_LOADED,

				/// Triggered when an hkaiNavVolume is unloaded.
			EVENT_VOLUME_UNLOADED,

				/// Triggered when an hkaiDirectedGraphExplicitCost is loaded.
			EVENT_GRAPH_LOADED,

				/// Triggered when an hkaiDirectedGraphExplicitCost is unloaded.
			EVENT_GRAPH_UNLOADED,
		};

		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

		ReplayEvent( ReplayEventType et )
			: m_eventType(et)
		{}

		virtual ~ReplayEvent() {}
		
			/// Serialization constructor.
		ReplayEvent( hkFinishLoadedObjectFlag f) : hkReferencedObject(f) {}
		
		const hkEnum<ReplayEventType, hkUint8> m_eventType;
	};

		/// Triggered when a world is first connected with the viewer.
	class WorldConnectedEvent : public ReplayEvent
	{
	public:

		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

		WorldConnectedEvent();
		
		WorldConnectedEvent( hkFinishLoadedObjectFlag f );
		
		~WorldConnectedEvent();
		
			/// The hkaiWorld that's being set
		hkRefPtr<hkaiWorld> m_world;
	};

		/// Triggered when an hkaiNavMeshInstance is loaded.
	class InstanceLoadedEvent : public ReplayEvent
	{
	public:

		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

		InstanceLoadedEvent();

		InstanceLoadedEvent(hkFinishLoadedObjectFlag f);

		~InstanceLoadedEvent();

			/// The instance being loaded.
		hkRefPtr<hkaiNavMeshInstance> m_instance;

			/// The corresponding mediator.
		hkRefPtr<hkaiNavMeshQueryMediator> m_mediator;

			/// The optional coarse graph used for hierarchical searches.
		hkRefPtr<hkaiDirectedGraphInstance> m_graph;
	};

		/// Triggered before silhouettes are stepped by the hkaiWorld.
	class SilhouettesSteppedEvent : public ReplayEvent
	{
	public:
		// +version(2)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);

		SilhouettesSteppedEvent();

		SilhouettesSteppedEvent(hkFinishLoadedObjectFlag f);
		
		~SilhouettesSteppedEvent();

			/// How the world was stepped (single or multithreaded). 
		hkEnum<hkaiWorld::StepThreading, hkUint32> m_stepThreading; //+default( hkaiWorld::STEP_SINGLE_THREADED )
		
			/// Generators being stepped this frame.
		hkArray< hkRefPtr<hkaiSilhouetteGenerator> > m_generators;

			/// Transforms of the nav mesh instances
		hkArray< hkTransform > m_instanceTransforms;

			/// hkaiOverlapManager data for the frame.
		hkArray<hkaiOverlapManagerSection> m_overlapManagerSections;

			/// Which sections get stepped in the overlap manager.
		hkBitField m_updatedSections;
	};

		/// Triggered when an hkaiNavMeshInstance is unloaded.
	class InstanceUnloadedEvent : public ReplayEvent
	{
	public:
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		
		InstanceUnloadedEvent() 
		:	ReplayEvent(EVENT_INSTANCE_UNLOADED) 
		{}
		
		InstanceUnloadedEvent(hkFinishLoadedObjectFlag f) 
		:	ReplayEvent(f) 
		{}

			/// UID of the instance being unloaded.
		hkaiSectionUid m_sectionUid;
	};

		/// Triggered when an hkaiNavVolume is loaded.
	class VolumeLoadedEvent : public ReplayEvent
	{
	public:
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		
		VolumeLoadedEvent();
		
		VolumeLoadedEvent(hkFinishLoadedObjectFlag f);

		~VolumeLoadedEvent();
		
			/// hkaiNavVolume being loaded
		hkRefPtr<hkaiNavVolumeInstance> m_volume;
		
			/// Corresponding mediator for the nav volume
		hkRefPtr<hkaiNavVolumeMediator> m_mediator;
	};

		/// Triggered when an hkaiNavVolume is unloaded.
	class VolumeUnloadedEvent : public ReplayEvent
	{
	public:
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		
		VolumeUnloadedEvent()
		:	ReplayEvent(EVENT_VOLUME_UNLOADED)
		{}
		
		VolumeUnloadedEvent(hkFinishLoadedObjectFlag f)
		:	ReplayEvent(f)
		{}

			/// UID of the volume being unloaded.
		hkaiSectionUid m_sectionUid;
	};

		/// Triggered when an hkaiDirectedGraphExplicitCost is loaded.
	class GraphLoadedEvent : public ReplayEvent
	{
	public:
		//+version(1)
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		
		GraphLoadedEvent();
		
		GraphLoadedEvent(hkFinishLoadedObjectFlag f);

		~GraphLoadedEvent();
		
			/// Graph being loaded
		hkRefPtr<hkaiDirectedGraphInstance> m_graph;
	};

		/// Triggered when an hkaiDirectedGraphExplicitCost is unloaded.
	class GraphUnloadedEvent : public ReplayEvent
	{
	public:
		HK_DECLARE_REFLECTION();
		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AI);
		
		GraphUnloadedEvent()
		:	ReplayEvent(EVENT_GRAPH_UNLOADED)
		{}
		
		GraphUnloadedEvent(hkFinishLoadedObjectFlag f)
		:	ReplayEvent(f)
		{}

			/// UID of the volume being unloaded.
		hkaiSectionUid m_sectionUid;
	};

		/// Applies the replay event to the given hkaiWorld.
		/// These pointers should initiall be HK_NULL and will be set if a WorldConnectedEvent is received.
		/// It is up to the user to remove the final reference from these when all events are finished.
		/// The hkJobQueue and hkJobThreadPool are used to step the silhouettes in multithreaded mode if the original step was also multithreaded.
	static void HK_CALL applyEventToWorld( hkaiSilhouetteRecorder::ReplayEvent& e, hkaiWorld*& world, hkJobQueue* jobQueue, hkJobThreadPool* threadPool );

public: // interface
	
		/// Whether or not the world information has been sent already.
	virtual bool isWorldInfoSent( const hkaiWorld* world ) const = 0;

		/// Called after the world information is sent. Responsible for making sure that subsequent calls to isWorldInfoSent() return true.
	virtual void sentWorldInfo( const hkaiWorld* world ) = 0;

		/// Handle an event, e.g. save it to a stream, or apply it to another world.
	virtual void sendEvent( const ReplayEvent& e) = 0;

protected:

	//
	// hkaiWorld::Listener interface
	//

	void navMeshInstanceAdded( const class hkaiWorld* world, hkaiNavMeshInstance* navMeshInstance, const hkaiNavMeshQueryMediator* mediator, hkaiDirectedGraphInstance* hierarchyGraph ) HK_OVERRIDE;
	void navMeshInstanceRemoved( const class hkaiWorld* world, hkaiNavMeshInstance* navMeshInstance, hkaiDirectedGraphInstance* hierarchyGraph ) HK_OVERRIDE;
	void navVolumeAdded( const class hkaiWorld* world, const class hkaiNavVolumeInstance* volume, const class hkaiNavVolumeMediator* mediator ) HK_OVERRIDE;
	void navVolumeRemoved( const class hkaiWorld* world, const class hkaiNavVolumeInstance* volume ) HK_OVERRIDE;
	void graphAdded( const class hkaiWorld* world, const class hkaiDirectedGraphInstance* graph ) HK_OVERRIDE;
	void graphRemoved( const class hkaiWorld* world, const class hkaiDirectedGraphInstance* graph ) HK_OVERRIDE;
	void preSilhouetteStepCallback(const hkaiWorld* world, hkaiWorld::StepThreading threading, const class hkBitField* sectionsToStep ) HK_OVERRIDE;

		/// Send the initial world information.
	void connectWorld( const hkaiWorld* world  );


};


	/// VDB viewer that records events to the movie stream.
class hkaiSilhouetteReplayViewer :	public hkaiViewerBase, 
									public hkaiSilhouetteRecorder
{
public:
	HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_BASE);

		/// Registers hkaiPathRequestViewer with the hkViewerFactory.
	static void HK_CALL registerViewer();

		/// Returns the viewer name. This text appears in the VDB client viewer menu.
	static inline const char* HK_CALL getName() { return "AI - Silhouette Replay"; }

		/// Creates a hkaiPathRequestViewer.
	static hkProcess* HK_CALL create(const hkArray<hkProcessContext*>& contexts);

	//
	// hkProcess interface
	//
	void init();

		/// Returns the tag associated with this viewer type.
		/// This is passed to the drawing calls so that the VDB knows from which viewer those calls came.
	virtual int getProcessTag() HK_OVERRIDE { return m_tag; }

	//
	// hkaiViewerContextWorldListener interface
	//
	virtual void worldAddedCallback( hkaiWorld* newWorld ) HK_OVERRIDE;
	virtual void worldRemovedCallback( hkaiWorld* oldWorld ) HK_OVERRIDE;

	//
	// hkaiSilhouetteRecorder interface
	//
	virtual bool isWorldInfoSent( const hkaiWorld* world ) const HK_OVERRIDE;
	virtual void sentWorldInfo( const hkaiWorld* world ) HK_OVERRIDE;
	virtual void sendEvent( const ReplayEvent& e) HK_OVERRIDE;

protected:

	hkaiSilhouetteReplayViewer(const hkArray<hkProcessContext*>& contexts);
	virtual ~hkaiSilhouetteReplayViewer();

protected:

		/// Used for isWorldInfoSent/sentWorldInfo
	hkBool m_worldSent;

	static int m_tag;
};

#endif

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
