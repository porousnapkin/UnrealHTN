# HTN Planner Plugin for Unreal Engine 5 - Implementation Plan

After analyzing the provided specification for an HTN Planner Plugin for Unreal Engine 5, I've developed a detailed, step-by-step implementation plan. The plan breaks down the development process into manageable chunks that build incrementally on each other, ensuring steady progress without major complexity jumps.

## Overall Development Blueprint

1. **Core Architecture** - Foundation classes and interfaces
2. **Basic Tasks and Planning** - Simple implementations of tasks and planning algorithm 
3. **Plan Execution** - Mechanisms for executing plans
4. **Enhanced Planning** - Advanced planning features like preconditions and effects
5. **Sensor System** - Systems for monitoring world state changes
6. **UE5 Integration - Navigation** - Connecting to UE5's navigation system
7. **UE5 Integration - Perception** - Linking with UE5's perception system
8. **UE5 Integration - SmartObjects** - Integration with UE5's SmartObjectSystem
9. **Visual Editing** - Basic graph editor implementation
10. **Enhanced Visual Editing** - Advanced editor features
11. **Blueprint Support** - Exposing functionality to Blueprint
12. **Debugging and Polish** - Tools for debugging and optimization

## Detailed Implementation Steps

### Phase 1: Core Architecture

This phase establishes the fundamental structure of the plugin.

#### Step 1.1: Basic Plugin Structure

```
Create a new UE5 plugin for an HTN planning system. Start by setting up the basic plugin structure with the following specifications:

1. Create a plugin named "HTNPlanner" with modules for runtime and editor
2. Set up the build.cs files for both modules with appropriate dependencies
3. Create basic plugin header and implementation files
4. Set up a plugin descriptor with appropriate metadata
5. Initialize the plugin modules with the necessary UE5 module lifecycle methods

The plugin should follow UE5's standard plugin architecture with a focus on maintainability and extensibility. Ensure proper namespacing and include the basic copyright notices and documentation.

The runtime module should depend on Core, CoreUObject, Engine, and AIModule.
The editor module should additionally depend on UnrealEd, Kismet, and BlueprintGraph.

Do not implement any HTN-specific functionality yet, just the correct plugin structure.
```

#### Step 1.2: Core World State Interface

```
Building on the HTNPlanner plugin structure created previously, implement the core world state interface that will represent the planning state for the HTN system.

1. Create an interface class IHTNWorldState that defines methods to:
   - Get/Set property values by name
   - Check if a property exists
   - Clone the world state
   - Compare world states for equality
   - Create a difference between world states

2. Define basic property types that can be stored in the world state:
   - Boolean values
   - Integer values
   - Float values
   - String values
   - Name/Object references

3. Create a struct FHTNProperty that can hold any of these types with appropriate type safety

The world state will function similar to UE5's Blackboard system but optimized for planning operations. It needs to be efficiently copyable since the planning process will create many world state copies during plan generation.

Focus on a clean interface design that will be implemented in the next step. Follow UE5 naming conventions and use appropriate UE5 types (FName for keys, TOptional where appropriate, etc.).
```

#### Step 1.3: Core Task Interfaces

```
Continue building the HTN Planner plugin by creating the core task interfaces that define the planning hierarchy.

1. Create an interface class IHTNTask that defines:
   - A method to decompose the task into subtasks given a world state
   - A method to check if the task is applicable in a given world state
   - A method to get the expected effects of the task on the world state
   - A method to get a human-readable description of the task

2. Create an enum EHTNTaskStatus with values:
   - InProgress
   - Succeeded
   - Failed
   - Invalid

3. Create an interface IHTNPrimitiveTask that extends IHTNTask with:
   - A method to execute the task with a given world state
   - A method to check if execution is complete
   - A method to get the current execution status

4. Create an interface IHTNCompoundTask that extends IHTNTask with:
   - A method to get available methods (alternative decompositions)
   - A method to apply a selected method to a world state

These interfaces should follow UE5 conventions for interfaces (prefixed with "I") and should use appropriate UE5 types for parameters and return values. Include good documentation comments for each method explaining its purpose and expected behavior.

The interfaces should be designed to support both C++ and Blueprint implementations later.
```

#### Step 1.4: Simple World State Implementation

```
Based on the IHTNWorldState interface created earlier, implement a concrete world state class for the HTN Planner.

1. Create a class FHTNWorldState that implements IHTNWorldState with:
   - A TMap<FName, FHTNProperty> to store properties
   - Implementation of all interface methods
   - Efficient copying behavior (move semantics where appropriate)
   - Serialization support for debugging and saving

2. Implement the property system with:
   - Type-safe getters and setters with template specializations
   - Proper error handling for type mismatches
   - Default values for missing properties
   - Comparison operators for different property types

3. Add a method to create a debug string representation of the world state

4. Create a simple unit test to verify basic world state operations:
   - Setting and getting different property types
   - Cloning a world state
   - Comparing world states
   - Creating difference between states

Follow UE5 best practices for memory management and use UE5's reflection system where appropriate. The implementation should be efficient for frequent copying during planning operations.
```

#### Step 1.5: Task Base Class

```
Now that we have a world state implementation, create the base task class for the HTN planning system.

1. Create an abstract class UHTNTaskBase that:
   - Is derived from UObject to support UE5's reflection system
   - Implements the IHTNTask interface
   - Provides common functionality for all task types
   - Has a unique identifier property (FGuid)
   - Has a human-readable name and description
   - Includes debug visualization helpers

2. Add the following properties and methods:
   - A name property (FName)
   - A description property (FString)
   - A cost property (float) for task prioritization
   - A debug visualization color
   - A virtual method for task validation that derived classes can override
   - A method to get a string representation for logging

3. Implement base versions of the IHTNTask methods that can be overridden by derived classes

The class should use UPROPERTY macros for properties that need to be exposed to the editor and use appropriate UE5 categories for organization. Include detailed documentation comments explaining the task lifecycle and how derived classes should implement the abstract methods.

This base class will serve as the foundation for both primitive and compound tasks that will be implemented in subsequent steps.
```

### Phase 2: Basic Tasks and Planning

This phase implements the basic task types and planning algorithm.

#### Step 2.1: Primitive Task Base

```
Building on the UHTNTaskBase class, implement the base class for primitive tasks.

1. Create a class UHTNPrimitiveTaskBase that:
   - Inherits from UHTNTaskBase
   - Implements the IHTNPrimitiveTask interface
   - Provides a framework for tasks that can be directly executed
   - Has a clean separation between planning and execution concerns

2. Add the following components:
   - A TArray<UHTNCondition*> for preconditions
   - A TArray<UHTNEffect*> for effects
   - An execution status property (EHTNTaskStatus)
   - A virtual ExecuteTask method that derived classes will implement
   - A virtual TickTask method for ongoing execution
   - A virtual EndTask method for cleanup

3. Implement the IHTNPrimitiveTask interface methods:
   - IsApplicable should check all preconditions against a world state
   - GetExpectedEffects should return all defined effects
   - Execute should set up the execution context and call ExecuteTask
   - IsComplete should check if the execution status is not InProgress

4. Add a simple logging mechanism for task execution events

The class should be designed to support both synchronous and asynchronous task execution models. Use appropriate UE5 delegates for execution callbacks and UPROPERTY macros for editor-exposed properties.

For now, we'll use placeholder UHTNCondition and UHTNEffect forward declarations, which will be implemented in later steps.
```

#### Step 2.2: Compound Task Base

```
Continuing with the task hierarchy, implement the base class for compound tasks.

1. Create a class UHTNCompoundTaskBase that:
   - Inherits from UHTNTaskBase
   - Implements the IHTNCompoundTask interface
   - Provides a framework for tasks that decompose into other tasks
   - Supports multiple decomposition methods

2. Add the following components:
   - A TArray<UHTNMethod*> for decomposition methods
   - A method selection mechanism that prioritizes methods
   - A method for decomposing the task using a selected method
   - A debug visualization helper for decomposition trees

3. Implement the IHTNCompoundTask interface methods:
   - GetAvailableMethods should return valid methods for a given world state
   - ApplyMethod should decompose using a specified method
   - Decompose should find the best method and apply it

4. Create a simple UHTNMethod class that:
   - Contains a TArray<UHTNTaskBase*> for subtasks
   - Has a name and description
   - Has an applicability condition (placeholder for now)
   - Has a priority value for method selection

The compound task implementation should support recursive decomposition while preventing infinite recursion through proper validation. Use UPROPERTY macros for editor-exposed properties and include detailed documentation about the decomposition process.

For demonstration purposes, include a simple example of how a compound task might be configured with methods.
```

#### Step 2.3: Task Factory

```
Create a task factory system to manage task instantiation and registration for the HTN planner.

1. Implement a UHTNTaskFactory class that:
   - Provides static methods to create and register task types
   - Manages a registry of available task classes
   - Supports both C++ and Blueprint-defined tasks
   - Ensures type safety during task creation

2. Add the following functionality:
   - Registration methods for primitive and compound task classes
   - Task instantiation methods that create properly initialized tasks
   - Task validation to ensure created tasks are properly configured
   - Task categorization for editor organization

3. Create a simple task registry that:
   - Maintains lists of registered primitive and compound task types
   - Provides methods to query available task types
   - Supports runtime registration of new task types
   - Persists task type information between editor sessions

4. Implement a blueprint function library that exposes factory functions to Blueprint

The task factory should follow the singleton pattern but implemented in a UE5-friendly way. Use TSubclassOf for type safety and appropriate UE5 reflection macros to support Blueprint integration.

Include example usage demonstrating how to register and create both primitive and compound tasks.
```

#### Step 2.4: Simple Planner Interface

```
Define the core planner interface and data structures for the HTN planning system.

1. Create a struct FHTNPlan that represents a complete plan:
   - A TArray<UHTNPrimitiveTaskBase*> of primitive tasks in execution order
   - A float totalCost property for plan evaluation
   - Comparison operators for plan prioritization
   - Serialization support for saving/loading plans
   - Methods to get a string representation for debugging

2. Create an interface IHTNPlanner that defines:
   - A method to generate a plan given a world state and goal tasks
   - A method to validate if a plan is still valid given a current world state
   - A method to generate a partial plan that extends an existing plan
   - A method to configure planning parameters (search depth, timeout, etc.)

3. Define a struct FHTNPlannerResult that contains:
   - An FHTNPlan plan
   - A boolean success flag
   - An enum reason for failure if not successful
   - Metrics about the planning process (time taken, nodes explored, etc.)
   - Debug information about the planning process

4. Create a struct FHTNPlanningConfig with configurable parameters:
   - Maximum search depth
   - Planning timeout
   - Maximum number of plans to consider
   - Heuristic configuration options

These interfaces and structures should follow UE5 conventions and use appropriate UE5 types. Include detailed documentation explaining how planning requests should be formed and how to interpret planning results.
```

#### Step 2.5: Basic DFS Planner

```
Implement a basic depth-first search planner for the HTN system.

1. Create a class UHTNDFSPlanner that:
   - Implements the IHTNPlanner interface
   - Uses depth-first search to find valid plans
   - Supports basic plan validation
   - Handles planning timeouts

2. Implement the core planning algorithm:
   - A recursive decomposition function for compound tasks
   - A method selection mechanism that tries each method in priority order
   - A precondition checking system for primitive tasks
   - A plan validation process that checks the final plan

3. Add performance and debugging features:
   - Planning metrics collection (nodes explored, decomposition depth, etc.)
   - Timeout mechanism to prevent excessive planning time
   - Debug logging for each planning step
   - Plan comparison for selecting the best plan

4. Create a simple test scenario that:
   - Sets up a world state
   - Defines a goal task
   - Runs the planner
   - Validates the resulting plan

Follow UE5 best practices for recursion and memory management. Use smart pointers where appropriate and ensure proper cleanup of temporary objects. The implementation should be focused on correctness first, with optimization opportunities noted for future improvement.

Include detailed documentation about the planning algorithm and how it selects and constructs plans.
```

### Phase 3: Plan Execution

This phase focuses on executing the generated plans.

#### Step 3.1: Plan Representation

```
Enhance the plan representation to support execution and serialization.

1. Expand the FHTNPlan struct to include:
   - Execution metadata (current step, start time, etc.)
   - Plan status (not started, in progress, completed, failed)
   - Dependency information between tasks
   - Task parameters for execution
   - Task results from completed tasks

2. Add serialization support:
   - JSON serialization for debugging and logging
   - Binary serialization for efficient storage
   - Custom UE5 asset format for plan templates

3. Implement plan manipulation methods:
   - Merging two plans together
   - Extracting a subplan
   - Replacing parts of a plan
   - Finding specific tasks within a plan

4. Create visualization helpers:
   - A method to generate a graphical representation
   - A text description of the plan
   - Step-by-step execution preview

The enhanced plan representation should maintain backward compatibility with the previous implementation while adding features needed for robust execution. Use appropriate UE5 serialization methods and ensure thread safety for concurrent access.

Include examples of serialized plans in both JSON and binary formats.
```

#### Step 3.2: Simple Plan Executor

```
Create a system to execute HTN plans generated by the planner.

1. Implement a class UHTNPlanExecutor that:
   - Takes an FHTNPlan and executes its tasks in sequence
   - Manages the execution state of the current plan
   - Provides callbacks for plan and task events
   - Supports pausing, resuming, and aborting plan execution

2. Add the following functionality:
   - A Tick method that updates the current executing task
   - Methods to start, pause, resume, and stop plan execution
   - Event delegates for task start, completion, and failure
   - Plan completion and failure handling

3. Implement task execution:
   - Call Execute on the current primitive task
   - Monitor task completion through task status
   - Handle task failures appropriately
   - Move to the next task when current task completes

4. Add simple error recovery:
   - Task timeout detection
   - Basic error handling for failed tasks
   - Logging of execution progress and issues

The executor should work with the UE5 tick system and support both synchronous and latent task execution. Use appropriate UE5 delegates for callbacks and ensure proper cleanup when execution completes or is aborted.

Include a simple example demonstrating how to execute a plan with event handling.
```

#### Step 3.3: Execution Context

```
Implement an execution context to manage state and resources during plan execution.

1. Create a class FHTNExecutionContext that:
   - Maintains the current world state during execution
   - Provides access to the executing agent
   - Stores shared resources and references needed by tasks
   - Tracks execution history and metrics

2. Add the following components:
   - A reference to the agent executing the plan (AActor*)
   - A mutable copy of the world state
   - A task parameter dictionary for passing data between tasks
   - A history of executed tasks and their results
   - Performance metrics for execution time

3. Implement context manipulation methods:
   - Methods to read and write context variables
   - Stack-based context scoping for compound tasks
   - Context cloning for parallel execution branches
   - Context merging for rejoining parallel branches

4. Integrate with the plan executor:
   - Initialize the context at plan start
   - Update the context after each task execution
   - Provide context to tasks during execution
   - Clean up the context when plan execution completes

The execution context should follow UE5 patterns for managing shared state and use appropriate UE5 container types. Ensure thread safety if the execution might span multiple threads or be accessed concurrently.

Include examples of how tasks can use the execution context to share information.
```

#### Step 3.4: Execution Monitoring

```
Enhance the plan execution system with monitoring and reporting capabilities.

1. Create a class UHTNExecutionMonitor that:
   - Observes plan execution progress
   - Detects execution problems and failures
   - Reports execution metrics and statistics
   - Provides debugging information

2. Implement monitoring features:
   - Task execution time tracking
   - Execution progress reporting (percentage complete, etc.)
   - Stuck task detection (tasks taking too long)
   - Resource usage monitoring
   - Execution trace for debugging

3. Add reporting mechanisms:
   - Real-time status updates through delegates
   - Detailed execution reports at completion
   - Performance analytics for optimization
   - Visual debugging displays in-editor and in-game

4. Integrate with plan executor:
   - Register the monitor with the executor
   - Provide hooks for monitoring key execution events
   - Add configuration options for monitoring frequency
   - Include monitoring overhead optimization

The execution monitoring system should be optional and configurable to minimize performance impact in shipping builds. Use UE5's stats system for performance tracking and appropriate debug visualization tools for visual feedback.

Include examples of monitoring output and how to configure monitoring sensitivity.
```

#### Step 3.5: Basic Replanning

```
Implement a basic replanning system to handle execution failures and changes in the world state.

1. Create a class UHTNReplanningSystem that:
   - Monitors world state changes during execution
   - Detects when the current plan becomes invalid
   - Triggers replanning when necessary
   - Integrates the new plan with the current execution

2. Add the following functionality:
   - Periodic validation of the remaining plan
   - Condition monitoring for critical preconditions
   - Plan repair for minor inconsistencies
   - Full replanning for major world state changes

3. Implement replanning strategies:
   - Continue from current task with new plan
   - Restart planning from initial goals
   - Hybrid approach that preserves applicable parts of the plan
   - Priority-based replanning for urgent situations

4. Integrate with plan executor:
   - Register the replanning system with the executor
   - Provide configuration for replanning triggers
   - Add replanning events and callbacks
   - Handle execution during replanning

The replanning system should balance reactivity with stability, avoiding excessive replanning while ensuring plan validity. Use appropriate heuristics for determining when replanning is necessary and efficient algorithms for plan repair.

Include examples of scenarios that would trigger replanning and how the system responds.
```

### Phase 4: Enhanced Planning

This phase adds advanced planning features like preconditions and effects.

#### Step 4.1: Preconditions Framework

```
Implement a flexible precondition system for HTN tasks.

1. Create a base class UHTNCondition that:
   - Evaluates whether a condition is true for a given world state
   - Provides a textual description of the condition
   - Supports Blueprint extension
   - Includes debug visualization helpers

2. Implement common condition types:
   - UHTNPropertyCondition for checking world state properties
   - UHTNComparisonCondition for numeric comparisons
   - UHTNDistanceCondition for spatial relationships
   - UHTNTagCondition for gameplay tag checks
   - UHTNBlueprintCondition for custom Blueprint logic

3. Add compound condition operators:
   - UHTNAndCondition for logical AND
   - UHTNOrCondition for logical OR
   - UHTNNotCondition for logical NOT
   - UHTNSequenceCondition for ordered evaluation

4. Integrate with the task system:
   - Add conditions to primitive tasks
   - Implement condition checking during planning
   - Add visualization for condition evaluation
   - Create condition editing UI helpers (for later phases)

The condition system should be extensible and follow UE5 patterns for customization. Use UPROPERTY macros for editor-exposed properties and include detailed documentation about how to create custom conditions.

Include examples of different condition configurations and their evaluation results.
```

#### Step 4.2: Effects Framework

```
Implement an effects system to model how tasks modify the world state.

1. Create a base class UHTNEffect that:
   - Applies a change to a world state
   - Provides a textual description of the effect
   - Supports Blueprint extension
   - Includes debug visualization helpers

2. Implement common effect types:
   - UHTNSetPropertyEffect for setting world state properties
   - UHTNIncrementEffect for modifying numeric properties
   - UHTNToggleEffect for boolean properties
   - UHTNRemovePropertyEffect for removing properties
   - UHTNBlueprintEffect for custom Blueprint logic

3. Add effect features:
   - Conditional effects that only apply in certain states
   - Probabilistic effects with success chances
   - Delayed effects that occur after a time delay
   - Compound effects that apply multiple changes

4. Integrate with the task system:
   - Add effects to primitive tasks
   - Apply effects during planning
   - Verify effects during execution
   - Create effect editing UI helpers (for later phases)

The effects system should accurately model how actions change the world and follow UE5 patterns for customization. Use UPROPERTY macros for editor-exposed properties and include detailed documentation about how to create custom effects.

Include examples of different effect configurations and their results when applied to a world state.
```

#### Step 4.3: Method Selection Recording

```
Implement a method traversal recording system to improve planning efficiency.

1. Create a class FHTNMethodTraversalRecord (MTR) that:
   - Records the methods selected during planning
   - Stores the hierarchy of decomposition decisions
   - Provides information for plan comparison and prioritization
   - Supports serialization for debugging and analysis

2. Add the following components:
   - A tree structure representing decomposition decisions
   - Method selection history for each compound task
   - Priority information for each method choice
   - Metrics about the decomposition process

3. Implement MTR-based optimizations:
   - Plan prioritization based on method preferences
   - Efficient plan comparison using MTR differences
   - Exploration guidance for subsequent planning attempts
   - Learning from successful and failed plans

4. Integrate with the planner:
   - Record method selections during planning
   - Use MTR for plan ranking
   - Provide MTR visualization for debugging
   - Include MTR in planning results

The MTR system should enhance planning efficiency without significant overhead. Use efficient data structures for recording and comparing method selections and include performance considerations in the implementation.

Include examples of how MTR information improves plan selection and generation.
```

#### Step 4.4: Partial Planning Support

```
Extend the planning system to support partial planning and plan merging.

1. Enhance the UHTNDFSPlanner to:
   - Generate partial plans up to a specified horizon
   - Resume planning from a partially executed plan
   - Support incremental planning during execution
   - Handle uncertainty in long-term planning

2. Implement plan merging functionality:
   - Methods to combine partial plans
   - Validation of merged plan consistency
   - Resolution of conflicts between plans
   - Optimization of merged plans

3. Add progressive refinement:
   - Abstract planning for long-term goals
   - Detailed planning for immediate actions
   - Hierarchical plan representation
   - Dynamic refinement during execution

4. Integrate with the execution system:
   - Configure when partial planning should occur
   - Manage the transition between plan segments
   - Provide seamless execution across plan boundaries
   - Handle replanning within the partial planning framework

The partial planning system should balance immediate reactivity with long-term goal achievement. Use appropriate abstractions for different planning horizons and ensure efficient transitions between plan segments.

Include examples of how partial planning handles complex scenarios where full planning would be impractical.
```

#### Step 4.5: Advanced Planning Options

```
Implement advanced configuration options and optimizations for the HTN planning system.

1. Enhance FHTNPlanningConfig with:
   - Planning algorithms selection (DFS, A*, etc.)
   - Heuristic weighting parameters
   - Search space pruning options
   - Anytime planning configuration
   - Parallel planning options

2. Implement planning optimizations:
   - Caching of decomposition results
   - Reuse of previous planning efforts
   - Progressive deepening for time-bounded planning
   - Branch-and-bound pruning of search space

3. Add domain-specific planning enhancements:
   - Landmark-based planning
   - State abstraction for complex domains
   - Critical-path analysis
   - Goal ordering and prioritization

4. Create a UHTNPlannerProfile asset type:
   - Encapsulates planning configuration
   - Provides presets for different agent types
   - Supports runtime switching between profiles
   - Includes documentation and usage examples

The advanced planning options should make the system adaptable to different domains and performance requirements. Use appropriate UE5 configuration systems and ensure that options are well-documented.

Include examples of how different configurations affect planning performance and results.
```

### Phase 5: Sensor System

This phase implements the system for monitoring world state changes.

#### Step 5.1: Sensor Base Class

```
Implement the base sensor system for monitoring world state changes.

1. Create a class UHTNSensorBase that:
   - Periodically checks aspects of the game world
   - Updates the world state based on observations
   - Notifies interested systems about significant changes
   - Supports Blueprint extension

2. Add the following functionality:
   - A configurable update interval
   - Priority-based updating (more important sensors update more frequently)
   - Event-based updating (update on specific game events)
   - Resource-aware scheduling (minimize performance impact)

3. Implement the sensor lifecycle:
   - Initialization with configuration parameters
   - Activation when monitoring starts
   - Periodic or event-triggered sensing
   - Deactivation when monitoring ends
   - Cleanup of resources

4. Create a basic sensor interface:
   - A virtual Sense method that derived classes implement
   - Methods to access and modify the update configuration
   - Debugging helpers for sensor activity
   - Performance tracking for sensor operations

The sensor base class should follow UE5 patterns for recurring operations and use appropriate timers or tick functions. Ensure that sensors can be enabled/disabled dynamically and include performance considerations in the design.

Include examples of how to create a simple custom sensor and configure its update behavior.
```

#### Step 5.2: Sensor Manager

```
Create a manager system to coordinate HTN sensors.

1. Implement a class UHTNSensorManager that:
   - Registers and manages multiple sensors
   - Schedules sensor updates efficiently
   - Distributes world state changes to interested systems
   - Provides debugging and monitoring tools

2. Add the following functionality:
   - Dynamic sensor registration and unregistration
   - Priority-based update scheduling
   - Batched processing of sensor updates
   - Resource usage limiting for performance

3. Implement integration with other systems:
   - Connect sensors to the world state
   - Notify planners of relevant changes
   - Provide sensor data to task execution
   - Support debugging visualization

4. Add configuration options:
   - Global update frequency scaling
   - Sensor category enabling/disabling
   - Performance budgeting for sensor updates
   - Logging level for sensor activities

The sensor manager should efficiently coordinate multiple sensors and minimize performance overhead. Use appropriate UE5 patterns for managing subsystems and ensure thread safety if sensors might update concurrently.

Include examples of how the sensor manager schedules and processes multiple sensors with different priorities.
```

#### Step 5.3: Basic Sensor Types

```
Implement a set of common sensor types for the HTN system.

1. Create the following sensor classes:
   - UHTNPropertySensor for monitoring world state properties
   - UHTNTimeSensor for time-based events
   - UHTNDistanceSensor for spatial relationships
   - UHTNEventSensor for game event monitoring
   - UHTNBlueprintSensor for custom Blueprint logic

2. Implement for each sensor:
   - Specific configuration properties
   - The Sense method implementation
   - World state update logic
   - Appropriate debug visualization

3. Add specialized functionality:
   - Threshold-based detection for numeric properties
   - Hysteresis to prevent rapid oscillation
   - Spatial grid optimization for distance sensors
   - Event filtering for event sensors

4. Create Blueprint-friendly versions:
   - Blueprint-native sensor base class
   - Blueprint-callable configuration methods
   - Event dispatchers for sensor triggers
   - Visual debugging components

The basic sensor types should cover common monitoring needs and follow consistent patterns. Use appropriate UE5 types for each sensor's domain and include performance optimizations where possible.

Include examples of how to configure and use each sensor type with expected world state updates.
```

#### Step 5.4: Sensor Integration with Planning

```
Integrate the sensor system with planning and execution components.

1. Enhance the UHTNReplanningSystem to:
   - Subscribe to relevant sensor updates
   - Evaluate sensor data against the current plan
   - Trigger replanning when critical changes are detected
   - Prioritize replanning based on sensor importance

2. Implement sensor-based preconditions:
   - Create UHTNSensorCondition for sensor-specific checks
   - Add sensor queries to the planning process
   - Cache sensor data during planning for consistency
   - Validate plans against latest sensor data

3. Add sensor debugging tools:
   - Visualization of sensor coverage and activity
   - Logging of sensor-triggered replanning
   - Analysis of sensor impact on planning
   - Performance monitoring for sensor operations

4. Create sensor configuration profiles:
   - Predefined sensor sets for common agent types
   - Balancing between reactivity and performance
   - Documentation for sensor selection guidelines
   - Examples of effective sensor configurations

The integration should create a responsive yet efficient planning system that reacts appropriately to world changes. Use appropriate event systems for notification and ensure that sensor processing doesn't become a performance bottleneck.

Include examples of how sensors trigger replanning in different scenarios and how to configure sensor sensitivity.
```

### Phase 6: UE5 Integration - Navigation

This phase connects the HTN system to UE5's navigation features.

#### Step 6.1: Navigation Request Task

```
Implement navigation-specific primitive tasks for the HTN system.

1. Create a class UHTNMoveToTask that:
   - Inherits from UHTNPrimitiveTaskBase
   - Interfaces with UE5's navigation system
   - Moves an agent to a specified location
   - Handles navigation success and failure

2. Add the following features:
   - Configuration for movement parameters (speed, acceptance radius, etc.)
   - Dynamic target selection from world state
   - Path following behavior options
   - Interruption handling during movement

3. Implement the execution logic:
   - Initialize navigation request in ExecuteTask
   - Monitor path following in TickTask
   - Handle completion and failure appropriately
   - Clean up navigation resources in EndTask

4. Add navigation-specific conditions:
   - UHTNPathExistsCondition to check if a path exists
   - UHTNDistanceCondition for proximity checks
   - UHTNNavigableCondition to verify locations are navigable
   - UHTNBetterPathCondition for path optimization

The navigation tasks should use UE5's AIModule components correctly and follow best practices for navigation requests. Ensure proper error handling for navigation failures and include appropriate debugging visualization.

Include examples of how to configure and use the movement task within a plan.
```

#### Step 6.2: Navigation Result Handling

```
Enhance navigation tasks with improved result handling and recovery.

1. Improve UHTNMoveToTask with:
   - Detailed failure categorization (no path, blocked path, etc.)
   - Partial success handling for interrupted movement
   - Recovery options for navigation failures
   - Performance optimizations for path requests

2. Implement navigation event handling:
   - React to dynamic obstacles during movement
   - Handle path updates from the navigation system
   - Process agent avoidance events
   - Respond to navigation mesh changes

3. Add advanced path following:
   - Custom movement styles (careful, rushed, stealthy)
   - Formation movement for groups
   - Corridor following with space awareness
   - Special movement capabilities (jumping, climbing)

4. Create navigation result reporting:
   - Path quality metrics
   - Navigation performance statistics
   - Debug visualization of paths and issues
   - Logging of navigation decisions

The enhanced navigation handling should make movement tasks robust and adaptable to changing environments. Use appropriate UE5 navigation callbacks and ensure good integration with the AIModule.

Include examples of how the system handles various navigation challenges and recovers from failures.
```

#### Step 6.3: Advanced Navigation Tasks

```
Implement specialized navigation tasks for complex movement patterns.

1. Create the following navigation task classes:
   - UHTNPatrolTask for following patrol routes
   - UHTNFollowActorTask for dynamic target following
   - UHTNFormationMoveTask for group movement
   - UHTNStealthMoveTask for detection-aware movement

2. Implement for each task:
   - Specific configuration properties
   - Specialized navigation request handling
   - Task-specific success and failure conditions
   - Appropriate debug visualization

3. Add specialized functionality:
   - Route generation for patrol tasks
   - Prediction for follow tasks
   - Formation slot assignment for group movement
   - Visibility and noise awareness for stealth

4. Create navigation-specific compounds tasks:
   - UHTNNavigateCompoundTask with methods for different movement styles
   - UHTNExploreCompoundTask for area exploration
   - UHTNSearchCompoundTask for target finding
   - UHTNEscapeCompoundTask for retreating

The advanced navigation tasks should build on the base movement capabilities while adding specialized behaviors. Use appropriate UE5 navigation systems and ensure good performance even with many agents.

Include examples of how these specialized tasks can be combined to create complex movement behaviors.
```

### Phase 7: UE5 Integration - Perception

This phase connects the HTN system to UE5's perception system.

#### Step 7.1: Perception System Binding

```
Create integration between the HTN system and UE5's perception system.

1. Implement a class UHTNPerceptionComponent that:
   - Inherits from or wraps UE5's AIPerceptionComponent
   - Converts perception events to world state updates
   - Manages perception configuration for HTN agents
   - Provides a clean interface for HTN tasks

2. Add perception event handling:
   - Process sight perception events
   - Handle sound perception events
   - Process damage and touch events
   - Support custom perception types

3. Implement world state mapping:
   - Convert perceived actors to world state entries
   - Track perception confidence and freshness
   - Handle perception memory and forgetting
   - Manage team-shared perception information

4. Create perception-specific sensors:
   - UHTNSightSensor for vision-based detection
   - UHTNHearingSensor for sound detection
   - UHTNDamageSensor for damage awareness
   - UHTNTeamPerceptionSensor for shared information

The perception binding should make it easy for HTN agents to react to perceived stimuli. Use appropriate UE5 perception events and ensure good performance with multiple agents and stimuli.

Include examples of how perception events update the world state and trigger appropriate planning.
```

#### Step 7.2: Perception-Based Conditions

```
Implement conditions that leverage perception data for decision making.

1. Create the following condition classes:
   - UHTNCanSeeCondition for visibility checks
   - UHTNCanHearCondition for audibility checks
   - UHTNPerceptionAgeCondition for information freshness
   - UHTNTeamAwarenessCondition for team knowledge

2. Implement for each condition:
   - Specific configuration properties
   - Condition evaluation logic using perception data
   - Appropriate debugging visualization
   - Performance optimizations for frequent checks

3. Add perception-specific parameters:
   - Confidence thresholds for uncertain perception
   - Maximum age for perception data
   - Team filtering for shared perception
   - Spatial relationships for directional perception

4. Create perception utility functions:
   - Blueprint library for common perception queries
   - Helper functions for perception-based decisions
   - Debugging tools for perception analysis
   - Visualization of perception conditions

The perception-based conditions should make it easy to create plans that respond to sensory information. Use appropriate UE5 perception types and ensure conditions can be efficiently evaluated during planning.

Include examples of how these conditions can be used to create reactive behaviors.
```

#### Step 7.3: EQS Integration

```
Integrate UE5's Environment Query System (EQS) with the HTN planning system.

1. Implement a class UHTNEQSTask that:
   - Inherits from UHTNPrimitiveTaskBase
   - Executes an EQS query during planning or execution
   - Stores query results in the world state
   - Provides access to EQS results for subsequent tasks

2. Add EQS-specific planning features:
   - Query execution during plan generation
   - Result filtering based on HTN context
   - Multi-query sequences for complex evaluations
   - Caching of query results for efficiency

3. Implement EQS-based conditions:
   - UHTNQueryResultsCondition for checking query results
   - UHTNBestResultCondition for optimal result verification
   - UHTNQueryScoreCondition for score-based decisions
   - UHTNLocationValidityCondition for position checking

4. Create EQS-specific compound tasks:
   - UHTNFindLocationTask for position selection
   - UHTNEvaluateTargetsTask for target prioritization
   - UHTNAssessEnvironmentTask for situation analysis
   - UHTNChooseApproachTask for approach planning

The EQS integration should leverage UE5's environmental understanding capabilities for HTN planning. Use appropriate EQS components and ensure good performance with complex queries.

Include examples of how EQS queries can enhance HTN planning with spatial awareness.
```

### Phase 8: UE5 Integration - SmartObjects

This phase connects the HTN system to UE5's SmartObjectSystem.

#### Step 8.1: SmartObject Claiming

```
Implement a system for HTN agents to claim and use SmartObjects.

1. Create a class UHTNSmartObjectClaimManager that:
   - Manages claims on SmartObjects by HTN agents
   - Handles claim lifetime and priority
   - Resolves conflicts between competing claims
   - Integrates with UE5's SmartObjectSubsystem

2. Implement claim operations:
   - Claim request and validation
   - Claim reservation and confirmation
   - Claim timeout and release
   - Claim priority and preemption

3. Add integration with planning:
   - Create claim-aware conditions
   - Implement claim effects for tasks
   - Add claim verification during execution
   - Handle failed claims during replanning

4. Create SmartObject-specific world state entries:
   - Available SmartObjects by type and capability
   - Claim status and ownership
   - Reservation timeouts and priorities
   - Claim history for learning

The SmartObject claiming system should enable coordination between multiple HTN agents. Use appropriate UE5 SmartObject interfaces and ensure thread safety for concurrent claim operations.

Include examples of how agents can coordinate their use of SmartObjects through claims.
```

#### Step 8.2: SmartObject Tasks

```
Implement tasks for interacting with SmartObjects in the HTN system.

1. Create a class UHTNSmartObjectTask that:
   - Inherits from UHTNPrimitiveTaskBase
   - Claims and uses a SmartObject during execution
   - Handles success and failure of SmartObject interactions
   - Releases claims appropriately when completed

2. Add SmartObject-specific task parameters:
   - SmartObject class or instance selection
   - Required capability specification
   - Interaction configuration
   - Timeout and retry settings

3. Implement SmartObject-specific conditions:
   - UHTNSmartObjectAvailableCondition for availability checks
   - UHTNSmartObjectCapabilityCondition for capability verification
   - UHTNSmartObjectDistanceCondition for proximity checks
   - UHTNSmartObjectClaimCondition for claim status verification

4. Create SmartObject compound tasks:
   - UHTNUseSmartObjectTask with methods for different usage patterns
   - UHTNFindSmartObjectTask for object selection
   - UHTNCoordinatedUseTask for multi-agent coordination
   - UHTNSmartObjectSearchTask for capability-based search

The SmartObject tasks should make it easy to incorporate SmartObject interactions into HTN plans. Use appropriate UE5 SmartObject interfaces and ensure good integration with the claim system.

Include examples of how these tasks can be used to create plans involving SmartObject interactions.
```

#### Step 8.3: Smart Environment Adaptation

```
Enhance the HTN system with adaptive behavior based on SmartObject availability.

1. Implement a class UHTNEnvironmentAdapter that:
   - Monitors the SmartObject environment
   - Updates the world state with environmental information
   - Triggers replanning when environment changes significantly
   - Provides adaptation recommendations for planning

2. Add environment analysis features:
   - SmartObject availability mapping
   - Capability density analysis
   - Agent congestion detection
   - Environmental change tracking

3. Implement adaptive planning strategies:
   - Alternative method selection based on environment
   - Dynamic task parameterization from environment
   - Opportunity detection and exploitation
   - Congestion avoidance and workload balancing

4. Create environment-specific compound tasks:
   - UHTNAdaptToEnvironmentTask with methods for different environments
   - UHTNExploitOpportunityTask for opportunistic behavior
   - UHTNAvoidCongestionTask for congestion management
   - UHTNOptimizeWorkloadTask for multi-agent efficiency

The environment adaptation system should make HTN agents responsive to their SmartObject environment. Use appropriate analysis techniques and ensure the adaptation doesn't cause excessive replanning.

Include examples of how agents adapt their behavior to changing SmartObject environments.
```

### Phase 9: Basic Visual Editing

This phase begins implementing the visual editor for HTN planning.

#### Step 9.1: Graph Data Model

```
Implement the graph data model for visually representing HTN hierarchies.

1. Create a class UHTNGraph that:
   - Represents the HTN hierarchy as a directed graph
   - Stores nodes for tasks, methods, conditions, and effects
   - Manages connections between nodes
   - Supports serialization and asset management

2. Add the following node types:
   - FHTNGraphNode_Task for task nodes
   - FHTNGraphNode_Method for method nodes
   - FHTNGraphNode_Condition for condition nodes
   - FHTNGraphNode_Effect for effect nodes

3. Implement graph operations:
   - Node creation and deletion
   - Connection management
   - Node property editing
   - Graph validation

4. Create serialization support:
   - Asset serialization for HTN graphs
   - Copy/paste functionality
   - Import/export capabilities
   - Version migration support

The graph data model should provide a foundation for visual editing while maintaining a clean mapping to the runtime HTN structures. Use appropriate UE5 asset management patterns and ensure robust serialization.

Include examples of how runtime HTN structures map to the graph representation.
```

#### Step 9.2: Basic Node Appearance

```
Implement the visual representation of HTN graph nodes.

1. Create a class SHTNGraphNode that:
   - Inherits from SGraphNode
   - Renders the visual appearance of HTN nodes
   - Handles selection and interaction
   - Displays relevant node information

2. Implement node type-specific appearances:
   - SHTNGraphNode_Task for task nodes
   - SHTNGraphNode_Method for method nodes
   - SHTNGraphNode_Condition for condition nodes
   - SHTNGraphNode_Effect for effect nodes

3. Add visual styling:
   - Node coloring by type and status
   - Icon display for node types
   - Status indicators for validation results
   - Size adjustment for content

4. Implement interaction features:
   - Selection handling
   - Drag and drop support
   - Context menu display
   - Pin management for connections

The node appearance classes should create a visually clear and intuitive representation of HTN elements. Use appropriate UE5 Slate widgets and follow UE5's visual style guidelines.

Include examples of how different node types appear in the editor.
```

#### Step 9.3: Connection Rules

```
Implement connection rules and validation for the HTN graph editor.

1. Create a class FHTNConnectionDrawingPolicy that:
   - Inherits from FConnectionDrawingPolicy
   - Renders connections between HTN nodes
   - Applies visual styling to different connection types
   - Provides feedback for valid/invalid connections

2. Implement connection validation:
   - Type compatibility checking
   - Cycle detection
   - Hierarchy validation
   - Connection limit enforcement

3. Add connection interaction:
   - Connection creation handling
   - Connection deletion
   - Connection rerouting
   - Connection selection

4. Create visual feedback:
   - Valid connection highlighting
   - Invalid connection indication
   - Connection direction visualization
   - Connection status indication

The connection rules should enforce the proper structure of HTN hierarchies while providing clear visual feedback. Use appropriate UE5 connection drawing techniques and ensure consistency with other UE5 graph editors.

Include examples of valid and invalid connections with their visual representations.
```

#### Step 9.4: Simple Property Editing

```
Implement property editing for HTN graph nodes.

1. Create a class FHTNGraphNodeDetails that:
   - Inherits from IDetailCustomization
   - Creates property editors for HTN nodes
   - Organizes properties into categories
   - Provides custom widgets for HTN-specific properties

2. Implement node type-specific detail panels:
   - FHTNTaskDetails for task properties
   - FHTNMethodDetails for method properties
   - FHTNConditionDetails for condition properties
   - FHTNEffectDetails for effect properties

3. Add custom property editors:
   - World state key selection
   - Task reference selection
   - Condition parameter editing
   - Effect configuration

4. Create inline editing features:
   - Direct editing of node titles
   - Quick property adjustment
   - Contextual property visibility
   - Validation feedback

The property editing system should make it easy to configure HTN elements without leaving the graph editor. Use appropriate UE5 property editing widgets and ensure consistent behavior with other UE5 editors.

Include examples of property editing for different node types.
```

### Phase 10: Enhanced Visual Editing

This phase adds advanced features to the visual editor.

#### Step 10.1: Node Categories and Organization

```
Implement node categorization and organization for the HTN editor.

1. Create a class FHTNGraphNodeFactory that:
   - Creates node widgets for different HTN element types
   - Organizes nodes into categories
   - Provides search and filtering capabilities
   - Manages node palette display

2. Implement category features:
   - Category definition and hierarchy
   - Visual styling for categories
   - Collapsible category groups
   - Category-based filtering

3. Add organizational tools:
   - Node comment functionality
   - Grouping of related nodes
   - Alignment and distribution tools
   - Graph overview navigation

4. Create node library features:
   - Favorites and recently used nodes
   - User-defined categories
   - Custom node templates
   - Library sharing

The node categorization should make it easy to find and use HTN elements in complex graphs. Use appropriate UE5 palette and organization patterns and ensure a clean visual representation.

Include examples of how categories organize different node types and how users can navigate the node library.
```

#### Step 10.2: Drag and Drop Support

```
Enhance the HTN editor with comprehensive drag and drop functionality.

1. Improve SHTNGraphNode with:
   - Drag source implementation
   - Drop target handling
   - Preview visualization during drag
   - Position snapping and alignment

2. Implement asset drag and drop:
   - Drag assets from content browser
   - Automatic node creation from assets
   - Configuration during drop
   - Default connection setup

3. Add graph element dragging:
   - Multi-selection dragging
   - Duplicating via drag
   - Reparenting through drag and drop
   - Ordering adjustment

4. Create advanced drop interactions:
   - Intelligent connection creation
   - Compound node formation
   - Template instantiation
   - Context-sensitive behavior

The drag and drop support should make HTN editing more efficient and intuitive. Use appropriate UE5 drag and drop interfaces and ensure good visual feedback during operations.

Include examples of different drag and drop operations and their results.
```

#### Step 10.3: Context Menus

```
Implement context-sensitive menus for the HTN editor.

1. Create a class FHTNGraphEditorContextMenu that:
   - Builds context menus for different selection states
   - Provides relevant operations for the current context
   - Organizes commands into logical categories
   - Includes keyboard shortcuts for common operations

2. Implement node-specific context menus:
   - Task-specific operations
   - Method-specific operations
   - Condition-specific operations
   - Effect-specific operations

3. Add graph-level context menus:
   - Selection operations
   - Layout commands
   - View options
   - Advanced editing tools

4. Create quick access features:
   - Most used commands promotion
   - Recent operations list
   - Context-sensitive suggestions
   - Customizable menu organization

The context menus should provide efficient access to relevant operations without overwhelming the user. Use appropriate UE5 menu building patterns and ensure consistency with other UE5 editors.

Include examples of different context menus in various editing scenarios.
```

#### Step 10.4: Advanced Property Editing

```
Implement advanced property editing features for HTN elements.

1. Enhance FHTNGraphNodeDetails with:
   - Multi-object editing support
   - Conditional property visibility
   - Custom property validation
   - Enhanced property visualization

2. Add specialized property editors:
   - World state key browser with search
   - Rich text editors for descriptions
   - Visual condition builders
   - Effect sequence editors

3. Implement reference browsing:
   - Find all references to a node
   - Display reference hierarchy
   - Edit references in-place
   - Validate reference integrity

4. Create visual property configuration:
   - Color pickers with presets
   - Curve and function editors
   - Asset browsers with preview
   - Drag handles for numeric properties

The advanced property editing should make it easy to configure complex HTN elements. Use appropriate UE5 property editing patterns and ensure a smooth workflow for common tasks.

Include examples of advanced property editing for complex HTN configurations.
```

### Phase 11: Blueprint Support

This phase exposes the HTN system to Blueprint.

#### Step 11.1: Blueprint Task Base Classes

```
Create Blueprint-friendly base classes for HTN tasks.

1. Implement a class UHTNBlueprintTaskBase that:
   - Inherits from UHTNTaskBase
   - Exposes task interfaces to Blueprint
   - Provides Blueprint-native event hooks
   - Includes Blueprint-friendly validation

2. Create Blueprint-specific task classes:
   - UHTNBlueprintPrimitiveTask for primitive tasks
   - UHTNBlueprintCompoundTask for compound tasks
   - UHTNBlueprintMethod for method definitions
   - UHTNBlueprintSensor for sensors

3. Add Blueprint execution support:
   - Blueprint event for task execution
   - Blueprint event for task ticking
   - Blueprint event for task completion
   - Blueprint-accessible execution context

4. Implement Blueprint validation:
   - Input parameter validation
   - Output parameter validation
   - Execution flow validation
   - Performance warning system

The Blueprint task classes should make it easy to create custom HTN elements in Blueprint while maintaining performance. Use appropriate UE5 Blueprint integration patterns and ensure good error handling for Blueprint-defined tasks.

Include examples of how to create custom tasks in Blueprint with their execution flow.
```

#### Step 11.2: Blueprint Conditions and Effects

```
Implement Blueprint-friendly condition and effect systems.

1. Create a class UHTNBlueprintConditionBase that:
   - Inherits from UHTNCondition
   - Exposes condition evaluation to Blueprint
   - Provides Blueprint-accessible context
   - Includes condition-specific debugging

2. Implement a class UHTNBlueprintEffectBase that:
   - Inherits from UHTNEffect
   - Exposes effect application to Blueprint
   - Provides Blueprint-accessible context
   - Includes effect-specific debugging

3. Add Blueprint-specific features:
   - Visual condition building tools
   - Effect preview in Blueprint editor
   - World state browser integration
   - Validation warnings for common issues

4. Create Blueprint library components:
   - Condition template library
   - Effect template library
   - Condition composition helpers
   - Effect sequence builders

The Blueprint condition and effect classes should make it easy to create custom logic in Blueprint. Use appropriate UE5 Blueprint integration patterns and ensure good performance for Blueprint-defined conditions and effects.

Include examples of how to create and use Blueprint conditions and effects in HTN tasks.
```

#### Step 11.3: Blueprint Function Library

```
Create a Blueprint function library for HTN operations.

1. Implement a class UHTNBlueprintFunctionLibrary that:
   - Provides static Blueprint-callable functions for HTN operations
   - Includes helper functions for common HTN tasks
   - Offers debugging and visualization utilities
   - Provides world state manipulation functions

2. Add planning-related functions:
   - Plan generation
   - Plan validation
   - Plan comparison
   - Plan statistics calculation

3. Implement world state functions:
   - World state creation and modification
   - World state comparison
   - Property access helpers
   - World state visualization

4. Create debugging helpers:
   - Plan visualization
   - Execution tracing
   - Performance analysis
   - Validation helpers

The Blueprint function library should make it easy to work with HTN components from Blueprint. Use appropriate UE5 Blueprint function library patterns and ensure good documentation for all functions.

Include examples of how to use the library functions in Blueprint graphs.
```

#### Step 11.4: Blueprint Graph Integration

```
Integrate HTN components with the Blueprint graph system.

1. Create custom Blueprint graph nodes:
   - UK2Node_GenerateHTNPlan for plan generation
   - UK2Node_ExecuteHTNPlan for plan execution
   - UK2Node_ModifyWorldState for world state manipulation
   - UK2Node_HTNTaskExecute for task execution

2. Implement node-specific features:
   - Pin configuration for different task types
   - Result splitting for different outcomes
   - Dynamic pin generation based on task parameters
   - Visual feedback for execution state

3. Add Blueprint compiler support:
   - Custom Blueprint compiler handling
   - Optimization for HTN operations
   - Warning generation for potential issues
   - Generated code documentation

4. Create Blueprint debug visualization:
   - Runtime plan visualization
   - Execution state display
   - World state monitoring
   - Performance metrics

The Blueprint graph integration should make it easy to incorporate HTN planning into larger Blueprint systems. Use appropriate UE5 Blueprint node patterns and ensure good error handling for node execution.

Include examples of how to use HTN nodes in Blueprint graphs and how they connect with other Blueprint components.
```

### Phase 12: Debugging and Polish

This phase adds debugging tools and final polish.

#### Step 12.1: Planning Visualization

```
Implement visual debugging tools for the HTN planning process.

1. Create a class UHTNPlanningDebugger that:
   - Visualizes the planning process step by step
   - Shows the search tree exploration
   - Highlights successful and failed branches
   - Provides metrics about the planning process

2. Implement visualization views:
   - Tree view of the decomposition process
   - Timeline view of planning steps
   - State comparison view for world states
   - Statistics view for performance metrics

3. Add interactive debugging:
   - Step-by-step planning execution
   - Breakpoints on specific decompositions
   - Manual exploration of alternative paths
   - Planning replay with different parameters

4. Create runtime debugging:
   - In-game planning visualization
   - Visual representation of current plan
   - Plan execution monitoring
   - Dynamic plan adjustment tools

The planning visualization should make it easy to understand and debug complex planning processes. Use appropriate UE5 debugging visualization patterns and ensure the tools are useful for both developers and designers.

Include examples of how to use the visualization tools to debug planning issues.
```

#### Step 12.2: World State Inspection

```
Implement tools for inspecting and debugging world states.

1. Create a class UHTNWorldStateDebugger that:
   - Displays world state contents in a readable format
   - Shows world state changes during planning and execution
   - Provides filtering and search for properties
   - Supports editing world state for testing

2. Implement visualization features:
   - Property categorization and grouping
   - Change highlighting for property modifications
   - Historical value tracking
   - Comparison between world states

3. Add runtime state monitoring:
   - Real-time world state display
   - Property change notifications
   - Critical property highlighting
   - Performance impact warnings

4. Create testing tools:
   - World state presets for testing
   - Property modification simulation
   - Condition evaluation testing
   - Effect application verification

The world state inspection tools should make it easy to understand and manipulate the HTN world state. Use appropriate UE5 property visualization patterns and ensure good performance even with large world states.

Include examples of how to use the inspection tools to debug world state issues.
```

#### Step 12.3: Enhanced Logging

```
Implement a comprehensive logging system for the HTN planner.

1. Create a class FHTNLogger that:
   - Provides categorized logging for HTN operations
   - Supports different verbosity levels
   - Includes context information in log entries
   - Offers filtering and searching capabilities

2. Add logging for key components:
   - Planning process logging
   - Execution logging
   - World state change logging
   - Sensor update logging

3. Implement log visualization:
   - Timeline view of log entries
   - Hierarchical log organization
   - Entry colorization by category and severity
   - Log entry correlation with game events

4. Create log analysis tools:
   - Pattern recognition in logs
   - Statistical analysis of planning and execution
   - Performance anomaly detection
   - Log comparison between runs

The enhanced logging system should provide detailed information about HTN operations while remaining efficient. Use appropriate UE5 logging patterns and ensure that logging can be controlled at runtime to minimize performance impact.

Include examples of log output for different HTN operations and how to configure logging levels.
```

#### Step 12.4: Performance Optimization

```
Implement performance optimization tools and techniques for the HTN system.

1. Create a class UHTNPerformanceMonitor that:
   - Tracks performance metrics for HTN operations
   - Identifies bottlenecks in planning and execution
   - Provides recommendations for optimization
   - Supports performance testing and comparison

2. Implement optimization strategies:
   - Caching for common planning operations
   - Task precomputation where possible
   - Lazy evaluation for conditions
   - Smart scheduling for sensors

3. Add performance configuration:
   - Scalability settings for different platforms
   - Performance profiles for different agent types
   - Dynamic adjustment based on frame budget
   - Override settings for specific scenarios

4. Create performance testing tools:
   - Benchmark scenarios for standard testing
   - Performance regression detection
   - Stress testing for large agent counts
   - Profile comparison between versions

The performance optimization should make the HTN system efficient even with many agents and complex plans. Use appropriate UE5 profiling tools and ensure that optimizations don't compromise correctness.

Include examples of performance metrics for different scenarios and how to interpret them for optimization.
```

#### Step 12.5: Documentation and Examples

```
Create comprehensive documentation and examples for the HTN planner plugin.

1. Implement in-editor documentation:
   - Integrated help system
   - Context-sensitive documentation
   - Interactive tutorials
   - Best practice guidelines

2. Create example content:
   - Sample HTN configurations for common agent types
   - Demonstration maps showing HTN behaviors
   - Tutorial projects with step-by-step guides
   - Performance benchmark scenarios

3. Add API documentation:
   - Detailed class and function documentation
   - Usage examples for key components
   - Integration guidelines
   - Common pitfall warnings

4. Create troubleshooting resources:
   - Common issue resolution guide
   - Debug checklist for planning problems
   - Performance troubleshooting guide
   - Community support channels

The documentation and examples should make it easy for developers to learn and use the HTN system. Use appropriate UE5 documentation formats and ensure that examples cover both basic and advanced usage.

Include a comprehensive getting started guide and reference documentation for all public APIs.
```

## Code-Generation LLM Prompts

### Phase 1: Core Architecture

#### Prompt 1.1: Basic Plugin Structure

```
Create a new UE5 plugin for an HTN planning system. Start by setting up the basic plugin structure with the following specifications:

1. Create a plugin named "HTNPlanner" with modules for runtime and editor
2. Set up the build.cs files for both modules with appropriate dependencies
3. Create basic plugin header and implementation files
4. Set up a plugin descriptor with appropriate metadata
5. Initialize the plugin modules with the necessary UE5 module lifecycle methods

The plugin should follow UE5's standard plugin architecture with a focus on maintainability and extensibility. Ensure proper namespacing and include the basic copyright notices and documentation.

The runtime module should depend on Core, CoreUObject, Engine, and AIModule.
The editor module should additionally depend on UnrealEd, Kismet, and BlueprintGraph.

Do not implement any HTN-specific functionality yet, just the correct plugin structure.
```

#### Prompt 1.2: Core World State Interface

```
Building on the HTNPlanner plugin structure created previously, implement the core world state interface that will represent the planning state for the HTN system.

1. Create an interface class IHTNWorldState that defines methods to:
   - Get/Set property values by name
   - Check if a property exists
   - Clone the world state
   - Compare world states for equality
   - Create a difference between world states

2. Define basic property types that can be stored in the world state:
   - Boolean values
   - Integer values
   - Float values
   - String values
   - Name/Object references

3. Create a struct FHTNProperty that can hold any of these types with appropriate type safety

The world state will function similar to UE5's Blackboard system but optimized for planning operations. It needs to be efficiently copyable since the planning process will create many world state copies during plan generation.

Focus on a clean interface design that will be implemented in the next step. Follow UE5 naming conventions and use appropriate UE5 types (FName for keys, TOptional where appropriate, etc.).
```

#### Prompt 1.3: Core Task Interfaces

```
Continue building the HTN Planner plugin by creating the core task interfaces that define the planning hierarchy.

1. Create an interface class IHTNTask that defines:
   - A method to decompose the task into subtasks given a world state
   - A method to check if the task is applicable in a given world state
   - A method to get the expected effects of the task on the world state
   - A method to get a human-readable description of the task

2. Create an enum EHTNTaskStatus with values:
   - InProgress
   - Succeeded
   - Failed
   - Invalid

3. Create an interface IHTNPrimitiveTask that extends IHTNTask with:
   - A method to execute the task with a given world state
   - A method to check if execution is complete
   - A method to get the current execution status

4. Create an interface IHTNCompoundTask that extends IHTNTask with:
   - A method to get available methods (alternative decompositions)
   - A method to apply a selected method to a world state

These interfaces should follow UE5 conventions for interfaces (prefixed with "I") and should use appropriate UE5 types for parameters and return values. Include good documentation comments for each method explaining its purpose and expected behavior.

The interfaces should be designed to support both C++ and Blueprint implementations later.
```

#### Prompt 1.4: Simple World State Implementation

```
Based on the IHTNWorldState interface created earlier, implement a concrete world state class for the HTN Planner.

1. Create a class FHTNWorldState that implements IHTNWorldState with:
   - A TMap<FName, FHTNProperty> to store properties
   - Implementation of all interface methods
   - Efficient copying behavior (move semantics where appropriate)
   - Serialization support for debugging and saving

2. Implement the property system with:
   - Type-safe getters and setters with template specializations
   - Proper error handling for type mismatches
   - Default values for missing properties
   - Comparison operators for different property types

3. Add a method to create a debug string representation of the world state

4. Create a simple unit test to verify basic world state operations:
   - Setting and getting different property types
   - Cloning a world state
   - Comparing world states
   - Creating difference between states

Follow UE5 best practices for memory management and use UE5's reflection system where appropriate. The implementation should be efficient for frequent copying during planning operations.
```

#### Prompt 1.5: Task Base Class

```
Now that we have a world state implementation, create the base task class for the HTN planning system.

1. Create an abstract class UHTNTaskBase that:
   - Is derived from UObject to support UE5's reflection system
   - Implements the IHTNTask interface
   - Provides common functionality for all task types
   - Has a unique identifier property (FGuid)
   - Has a human-readable name and description
   - Includes debug visualization helpers

2. Add the following properties and methods:
   - A name property (FName)
   - A description property (FString)
   - A cost property (float) for task prioritization
   - A debug visualization color
   - A virtual method for task validation that derived classes can override
   - A method to get a string representation for logging

3. Implement base versions of the IHTNTask methods that can be overridden by derived classes

The class should use UPROPERTY macros for properties that need to be exposed to the editor and use appropriate UE5 categories for organization. Include detailed documentation comments explaining the task lifecycle and how derived classes should implement the abstract methods.

This base class will serve as the foundation for both primitive and compound tasks that will be implemented in subsequent steps.
```

### Phase 2: Basic Tasks and Planning

#### Prompt 2.1: Primitive Task Base

```
Building on the UHTNTaskBase class, implement the base class for primitive tasks.

1. Create a class UHTNPrimitiveTaskBase that:
   - Inherits from UHTNTaskBase
   - Implements the IHTNPrimitiveTask interface
   - Provides a framework for tasks that can be directly executed
   - Has a clean separation between planning and execution concerns

2. Add the following components:
   - A TArray<UHTNCondition*> for preconditions
   - A TArray<UHTNEffect*> for effects
   - An execution status property (EHTNTaskStatus)
   - A virtual ExecuteTask method that derived classes will implement
   - A virtual TickTask method for ongoing execution
   - A virtual EndTask method for cleanup

3. Implement the IHTNPrimitiveTask interface methods:
   - IsApplicable should check all preconditions against a world state
   - GetExpectedEffects should return all defined effects
   - Execute should set up the execution context and call ExecuteTask
   - IsComplete should check if the execution status is not InProgress

4. Add a simple logging mechanism for task execution events

The class should be designed to support both synchronous and asynchronous task execution models. Use appropriate UE5 delegates for execution callbacks and UPROPERTY macros for editor-exposed properties.

For now, we'll use placeholder UHTNCondition and UHTNEffect forward declarations, which will be implemented in later steps.
```

#### Prompt 2.2: Compound Task Base

```
Continuing with the task hierarchy, implement the base class for compound tasks.

1. Create a class UHTNCompoundTaskBase that:
   - Inherits from UHTNTaskBase
   - Implements the IHTNCompoundTask interface
   - Provides a framework for tasks that decompose into other tasks
   - Supports multiple decomposition methods

2. Add the following components:
   - A TArray<UHTNMethod*> for decomposition methods
   - A method selection mechanism that prioritizes methods
   - A method for decomposing the task using a selected method
   - A debug visualization helper for decomposition trees

3. Implement the IHTNCompoundTask interface methods:
   - GetAvailableMethods should return valid methods for a given world state
   - ApplyMethod should decompose using a specified method
   - Decompose should find the best method and apply it

4. Create a simple UHTNMethod class that:
   - Contains a TArray<UHTNTaskBase*> for subtasks
   - Has a name and description
   - Has an applicability condition (placeholder for now)
   - Has a priority value for method selection

The compound task implementation should support recursive decomposition while preventing infinite recursion through proper validation. Use UPROPERTY macros for editor-exposed properties and include detailed documentation about the decomposition process.

For demonstration purposes, include a simple example of how a compound task might be configured with methods.
```

#### Prompt 2.3: Task Factory

```
Create a task factory system to manage task instantiation and registration for the HTN planner.

1. Implement a UHTNTaskFactory class that:
   - Provides static methods to create and register task types
   - Manages a registry of available task classes
   - Supports both C++ and Blueprint-defined tasks
   - Ensures type safety during task creation

2. Add the following functionality:
   - Registration methods for primitive and compound task classes
   - Task instantiation methods that create properly initialized tasks
   - Task validation to ensure created tasks are properly configured
   - Task categorization for editor organization

3. Create a simple task registry that:
   - Maintains lists of registered primitive and compound task types
   - Provides methods to query available task types
   - Supports runtime registration of new task types
   - Persists task type information between editor sessions

4. Implement a blueprint function library that exposes factory functions to Blueprint

The task factory should follow the singleton pattern but implemented in a UE5-friendly way. Use TSubclassOf for type safety and appropriate UE5 reflection macros to support Blueprint integration.

Include example usage demonstrating how to register and create both primitive and compound tasks.
```

#### Prompt 2.4: Simple Planner Interface

```
Define the core planner interface and data structures for the HTN planning system.

1. Create a struct FHTNPlan that represents a complete plan:
   - A TArray<UHTNPrimitiveTaskBase*> of primitive tasks in execution order
   - A float totalCost property for plan evaluation
   - Comparison operators for plan prioritization
   - Serialization support for saving/loading plans
   - Methods to get a string representation for debugging

2. Create an interface IHTNPlanner that defines:
   - A method to generate a plan given a world state and goal tasks
   - A method to validate if a plan is still valid given a current world state
   - A method to generate a partial plan that extends an existing plan
   - A method to configure planning parameters (search depth, timeout, etc.)

3. Define a struct FHTNPlannerResult that contains:
   - An FHTNPlan plan
   - A boolean success flag
   - An enum reason for failure if not successful
   - Metrics about the planning process (time taken, nodes explored, etc.)
   - Debug information about the planning process

4. Create a struct FHTNPlanningConfig with configurable parameters:
   - Maximum search depth
   - Planning timeout
   - Maximum number of plans to consider
   - Heuristic configuration options

These interfaces and structures should follow UE5 conventions and use appropriate UE5 types. Include detailed documentation explaining how planning requests should be formed and how to interpret planning results.
```

#### Prompt 2.5: Basic DFS Planner

```
Implement a basic depth-first search planner for the HTN system.

1. Create a class UHTNDFSPlanner that:
   - Implements the IHTNPlanner interface
   - Uses depth-first search to find valid plans
   - Supports basic plan validation
   - Handles planning timeouts

2. Implement the core planning algorithm:
   - A recursive decomposition function for compound tasks
   - A method selection mechanism that tries each method in priority order
   - A precondition checking system for primitive tasks
   - A plan validation process that checks the final plan

3. Add performance and debugging features:
   - Planning metrics collection (nodes explored, decomposition depth, etc.)
   - Timeout mechanism to prevent excessive planning time
   - Debug logging for each planning step
   - Plan comparison for selecting the best plan

4. Create a simple test scenario that:
   - Sets up a world state
   - Defines a goal task
   - Runs the planner
   - Validates the resulting plan

Follow UE5 best practices for recursion and memory management. Use smart pointers where appropriate and ensure proper cleanup of temporary objects. The implementation should be focused on correctness first, with optimization opportunities noted for future improvement.

Include detailed documentation about the planning algorithm and how it selects and constructs plans.
```

### Phase 3: Plan Execution

#### Prompt 3.1: Plan Representation

```
Enhance the plan representation to support execution and serialization.

1. Expand the FHTNPlan struct to include:
   - Execution metadata (current step, start time, etc.)
   - Plan status (not started, in progress, completed, failed)
   - Dependency information between tasks
   - Task parameters for execution
   - Task results from completed tasks

2. Add serialization support:
   - JSON serialization for debugging and logging
   - Binary serialization for efficient storage
   - Custom UE5 asset format for plan templates

3. Implement plan manipulation methods:
   - Merging two plans together
   - Extracting a subplan
   - Replacing parts of a plan
   - Finding specific tasks within a plan

4. Create visualization helpers:
   - A method to generate a graphical representation
   - A text description of the plan
   - Step-by-step execution preview

The enhanced plan representation should maintain backward compatibility with the previous implementation while adding features needed for robust execution. Use appropriate UE5 serialization methods and ensure thread safety for concurrent access.

Include examples of serialized plans in both JSON and binary formats.
```

#### Prompt 3.2: Simple Plan Executor

```
Create a system to execute HTN plans generated by the planner.

1. Implement a class UHTNPlanExecutor that:
   - Takes an FHTNPlan and executes its tasks in sequence
   - Manages the execution state of the current plan
   - Provides callbacks for plan and task events
   - Supports pausing, resuming, and aborting plan execution

2. Add the following functionality:
   - A Tick method that updates the current executing task
   - Methods to start, pause, resume, and stop plan execution
   - Event delegates for task start, completion, and failure
   - Plan completion and failure handling

3. Implement task execution:
   - Call Execute on the current primitive task
   - Monitor task completion through task status
   - Handle task failures appropriately
   - Move to the next task when current task completes

4. Add simple error recovery:
   - Task timeout detection
   - Basic error handling for failed tasks
   - Logging of execution progress and issues

The executor should work with the UE5 tick system and support both synchronous and latent task execution. Use appropriate UE5 delegates for callbacks and ensure proper cleanup when execution completes or is aborted.

Include a simple example demonstrating how to execute a plan with event handling.
```

Continue with more prompts for each phase and step as outlined above. Each prompt should build on the previous ones, following the detailed implementation plan.