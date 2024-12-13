Feature Overview
================


Graph object IDs
----------------

Nodes and Connections do not have any dedicated data instances. Instead, all the
relevant data is stored inside a user-defined ``GraphModel`` class inherited from
``AbstractGraphModel``.

::

  // Definitions.hpp
  using NodeId = unsigned int;

Each node is associated with a **unique** integer ``NodeId`` which is returned
from the function ``NodeId AbstractGraphModel::addNode(QString)``.

Important
  This is the responsibility of the model to generate unique ``NodeId`` s.


The ``ConnectionId`` is nothing else but a combination of input and output
``NodeId`` values with the corresponding ``PortIndex``:

::

  // Definitinos.hpp
  struct ConnectionId
  {
    NodeId    outNodeId;
    PortIndex outPortIndex;
    NodeId    inNodeId;
    PortIndex inPortIndex;
  };

Serialization
-------------

The serialization is supported at the moment by ``DataFlowGraphModel``. Should
you implement your own derivative of the class ``AbstractGraphModel``, it's up to
you to support the serialization using the existing helper functions. An example
of such a code could be found in ``src/DataFlowGraphModel.cpp``.

In order so save the whole scene we normally make a loop over all the nodes and
save their data in a Json arrray with a key "nodes" and then over all the
connections which are saved in an array "connections".

A typical node Json saved for the data flow scenario looks as follows:

::

  {
    "id" : 0,
    "internal-data" : {
      "model-name" : "Subtraction"
    },
    "position" : {
      "x" : -383,
      "y" : -95
    }
  }

The section ``internal-data`` is filled by the specific data of the node itself
and is created in the function ``DataFlowGraphModel::saveNode(NodeId)``. In the
data-propagation workflow we store a name of the model there. In your application
it could be any internal additional data, i.e. an internal node state.

The Json for a serialized Connection in this case looks very simple:

::

  {
    "inPortIndex" : 0,
    "intNodeId" : 1,
    "outNodeId" : 0,
    "outPortIndex" : 0
  }

The data above is produced by a function ``DataFlowGraphModel::saveConnection``.

Code Example
  See the function ``DataFlowGraphModel::save()`` in the file
  ``src/DataFlowGraphModel.cpp``.


Undo/Redo
---------

In order to support the undo/redo capabilities we employ the standar Qt's class
`QUndoStack <https://doc.qt.io/qt-6/qundostack.html>`_ . We keep a stack instance inside your ``BasicGraphicsScene`` (or its derivatives).

Some default ``QUndoCommand`` s are already implemented in the file
``src/QUndoCommands.cpp``

The command ``DeleteCommand`` uses serialization to store the information of the
removed objects, namely ``AbstractGraphModel::saveNode(NodeId)`` and
``AbstractGraphModel::saveConnection(ConnectionId)``. Make sure you override
these functions in your derived graph models.

Wrapping your Graph Structure
-----------------------------

If your task is to visualize a graph specific to your application and you do not
need the basic "data propagation" semantic implemented in this library
(``DataFlowGraphModel``, ``DataFlowGraphicsScene``), you might need to derive
from ``AbstractGraphModel`` and implement several abstract functions.

The models follows pretty much the ideas underlying the Qt's
``QAbstractItemModel``. The class delivers the IDs for all the existing scene
objects, the model is responsible for generating such unique IDs. You should
deliver information about nodes via ``AbstractGraphModel::nodeData`` and assign
the data to nodes using ``AbstractGraphModel::setNodeData``. The passed and
returned data is wrapped into the data type ``QVariant``

The pivotal ``enum`` that defines type of the information we need to obtain is
called ``NodeRole``. See the file ``include/QtNodes/internal/Definitions.hpp``.


.. table::
     :widths: 10 30

     ==================== ==========================
     NodeRole             Description
     ==================== ========================== 
     Type                 It corresponds to the type of the node and is described
                          by a ``QString`` value. For example, in the data-flow
                          models we return the name of the ``NodeDelegateModel``
                          that needs to be instantiated by
                          ``NodeDelegateModelRegistry``.

     Position             ``QPointF`` position of the node on the scene.

     Size                 ``QSize`` the size of the internal area of thet node.
                          Typically it is used by embedding a widged inside a
                          node.

     CaptionVisible       ``bool`` that defines whether to show a node's caption.

     Caption              ``QString`` defines whether to show a node's caption.

     Style                Node editor's internal json structure returned as a
                          ``QVariantMap`` that defines colors, gradients and
                          effects for the node painting

     InternalData         ``QJsonObject`` converted to ``QVariantMap`` that
                          serializes the iternal node's state.

     InPortCount          ``unsigned int`` -- the number of input ports.

     OutPortCount         ``unsigned int`` -- the number of output ports.

     Widget               ``QWidget*`` a pointer to allocated QWidget instance
                          that must be embedded into a node. If nothing is
                          embedded, it must be ``nullptr`` by default.
     ==================== ==========================

Code Example
  For the usage see ``examples/simple_graph_model``.


Node and Scene Styling
----------------------

Default Node, Connection, and GraphicsView styles are stored in a centrall class
``StyleCollection``.

Each default style is parsed from an internal Json string and stored in a
corresponding data-class. Below you'll find the contents of the Json strings at
the moment of writing this documentation.

**GraphicsViewStyle**

::

  {
    "GraphicsViewStyle": {
      "BackgroundColor": [53, 53, 53],
      "FineGridColor": [60, 60, 60],
      "CoarseGridColor": [25, 25, 25]
    }
  }


**NodeStyle**

::

  {
    "NodeStyle": {
      "NormalBoundaryColor": [255, 255, 255],
      "SelectedBoundaryColor": [255, 165, 0],
      "GradientColor0": "gray",
      "GradientColor1": [80, 80, 80],
      "GradientColor2": [64, 64, 64],
      "GradientColor3": [58, 58, 58],
      "ShadowColor": [20, 20, 20],
      "FontColor" : "white",
      "FontColorFaded" : "gray",
      "ConnectionPointColor": [169, 169, 169],
      "FilledConnectionPointColor": "cyan",
      "ErrorColor": "red",
      "WarningColor": [128, 128, 0],

      "PenWidth": 1.0,
      "HoveredPenWidth": 1.5,

      "ConnectionPointDiameter": 8.0,

      "Opacity": 0.8,
    }
  }


**ConnectionStyle**

::

  {
    "ConnectionStyle": {
      "ConstructionColor": "gray",
      "NormalColor": "darkcyan",
      "SelectedColor": [100, 100, 100],
      "SelectedHaloColor": "orange",
      "HoveredColor": "lightcyan",

      "LineWidth": 3.0,
      "ConstructionLineWidth": 2.0,
      "PointDiameter": 10.0,

      "UseDataDefinedColors": false
    }
  }

Code Example
  For the usage see ``examples/styles`` and ``examples/connection_colors``.

Vertical Layout
---------------

This feature might seem to be a bit "raw". I haven't had good use cases from real
life projects to polish the code and the resulting node layout and rendering.

The current node layout in a vertical mode looks as follows:

::

     -------o-------------o-------
    |  PortCaption   PortCaption  |
    |                             |
    |        Node Caption         |
    |                             |
    |                             |
    |         PortCaption         |
     --------------o--------------

Code Example
  For the usage see ``examples/vertical_layout``.


Dynamic Ports
-------------

Dynamic Ports operations are driven by functions of the class
``AbstractGraphModel``:

- ``AbstractGraphModel::portsAboutToBeDeleted``
- ``AbstractGraphModel::portsDeleted``
- ``AbstractGraphModel::portsAboutToBeInserted``
- ``AbstractGraphModel::portsInserted``


The function with the name "AboutTo" prepares the changes:

1. It computes the new connection IDs that are to be applied after the change is
   done.
2. It removes the existing connections that would have invalid addresses after
   modifications.

The functions ``porstDeleted`` and ``portsInserted`` create the new precomputed
connections with the correct IDs.

If you want to modify the number of ports in your code, it should approximately
as follows:

::

  void YourGraphModel::addPort()
  {
    portsAboutToBeInserted(nodeId, PortType::Out, 1, 2);


    // 在这里进行你的底层数据修改
    // 上面的函数调用已经为新的输出端口做好了准备，这些端口的索引是1和2。
    // 所有在新端口2以下的现有连接都将被删除并重新插入，其ID将偏移2位。
    //   DO YOUR UNDERLYING DATA MODIFICATIONS HERE
    //   The function call above has prepared the insertion of new output ports
    //   with the indexes 1 and 2.
    //   All the existed connectes below the new port 2 would be deleted and
    //   re-inserted with the new IDs (shifted by 2).

    porstInserted();
  }


Code Example
  For the usage see ``examples/dynamic_ports``.




冻结节点
Locked Nodes and Connections
----------------------------

It is possible to completely disable or "freeze" the nodes. This would make them
insensitive to moving and selecting events with the mouse.

In order to achieve such a behavior set specific flags and return from your graph
model:

::


  NodeFlags
  YourGraphModel::
  nodeFlags(NodeId nodeId) const override
  {
    auto basicFlags = DataFlowGraphModel::nodeFlags(nodeId);

    if (_nodesLocked)
      basicFlags |= NodeFlag::Locked;

    return basicFlags;
  }


禁用连接分离（Disabled Connection Detaching）
在某些情况下，您可能希望用户无法断开节点之间的连接，
例如为了保持数据流的完整性或防止误操作。这种功能可以通过设置特定标志或覆盖相关行为来实现。
Disabled Connection Detaching
-----------------------------


For disabling detaching the connections from certain nodes override the function
``virtual bool AbstractGraphModel::detachPossible(ConnectionId const) const``.
The default implementaion always returns ``true``.

Code Example
  For the usage see ``examples/lock_nodes_and_connections``.



Data Propagation
数据传播（Data Propagation）
----------------

数据传播类为基础的 AbstractGraphModel 添加了额外的功能，
使它们能够在创建连接时将数据从一个节点推送到另一个节点。
Data-propagating classes add extra funtionality to the basic
``AbstractGraphModel`` which allows them to push the data from node to node upon
creating a connection.


数据传播的流程从 NodeDelegateModel 实例开始。它会通过一个 Qt 信号 dataUpdated(PortIndex) 触发数据更新。
我们假设数据总是从右侧的输出端口（PortType::Out）发出。
The chain starts from the instance of a ``NodeDelegateModel``. It emits a Qt
signal ``dataUpdated(PortIndex)``. We always assume that the data is emitted from
one of the right hand side ports (``PortType::Out``).


然后，函数 DataFlowGraphModel::onOutPortDataUpdated(NodeId, PortIndex) 开始发挥作用。
该函数从输出端口读取数据，收集与指定 PortIndex 相关联的所有连接，
并通过 DataFlowGraphModel::setPortData 将数据设置到已连接的节点。
在通过 NodeDelegateModel::setInData(...) 将数据设置到目标节点的输入代理模型后，
我们会发出信号 inPortDataWasSet(nodeId, portType, portIndex)。
该信号用于重新绘制接收数据的节点，同时也可以供用户自定义用途进行挂接（例如触发其他功能）。
Then the function ``DataFlowGraphModel::onOutPortDataUpdated(NodeId, PortIndex)``
comes into play. It reads the data from the output port, collects all the
attached connections for the given ``PortIndex`` and sets the data to the
connected nodes using ``DataFlowGraphModel::setPortData``. After setting the data
to the input delegate model via ``NodeDelegateModel::setInData(...)`` we emit the
signal ``inPortDataWasSet(nodeId, portType, portIndex)``. The signal is used to
redraw the receiver node and could be hooked up for other user's purposes.

::

NodeDelegateModel::dataUpdated(PortIndex)

// 从源节点的 Delegate Model 发出更新信号 -> 源节点的 NodeId
DataFlowGraphModel::onOutPortDataUpdated(NodeId, PortIndex)

// 从源节点流向目标节点 -> 调用 setPortData
DataFlowGraphModel::setPortData()

// 在目标节点的 Delegate Model 中接收数据并更新
NodeDelegateModel::setInData(NodeData, portIndex)

// 通知数据更新并重新绘制目标节点
DataFlowGraphModel::setPortData()


Headless Mode
^^^^^^^^^^^^^

The class ``AbstractGraphModel`` is independent of any scenes or visualization
windows. It is possible to instantiate a descendant of this abstract class and
populate the graph model.

Any instantiated ``BasicGraphicsScene`` could be also used without attaching it
to a dedicated ``GraphicsView``.

Code Example
  See ``examples/calculator/headless_main.cpp``. In this file we instantiate just
  a ``DataFlowGraphModel`` and load a pre-saved calculator graph structure into
  it. The model is able to compute the results if the user modifies the inputs in
  the code.
