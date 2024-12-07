#pragma once

#include "Export.hpp"
#include "NodeData.hpp"
#include "NodeDelegateModel.hpp"
#include "QStringStdHash.hpp"

#include <QtCore/QString>

#include <functional>
#include <memory>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace QtNodes {

/// Class uses map for storing models (name, model)
class NODE_EDITOR_PUBLIC NodeDelegateModelRegistry
{
public:
    using RegistryItemPtr = std::unique_ptr<NodeDelegateModel>;
    using RegistryItemCreator = std::function<RegistryItemPtr()>;
    using RegisteredModelCreatorsMap = std::unordered_map<QString, RegistryItemCreator>;
    using RegisteredModelsCategoryMap = std::unordered_map<QString, QString>;
    using CategoriesSet = std::set<QString>;

    //using RegisteredTypeConvertersMap = std::map<TypeConverterId, TypeConverter>;

    NodeDelegateModelRegistry() = default;
    ~NodeDelegateModelRegistry() = default;

    NodeDelegateModelRegistry(NodeDelegateModelRegistry const &) = delete;
    NodeDelegateModelRegistry(NodeDelegateModelRegistry &&) = default;
    NodeDelegateModelRegistry &operator=(NodeDelegateModelRegistry const &) = delete;
    NodeDelegateModelRegistry &operator=(NodeDelegateModelRegistry &&) = default;

public:


template<typename ModelType>
void registerModel(RegistryItemCreator creator, QString const &category = "Nodes")
{
    QString const name = computeName<ModelType>(HasStaticMethodName<ModelType>{}, creator);

    // 检查是否该名称的模型已经注册过
    if (!_registeredItemCreators.count(name)) {
        // 如果没有注册过，则将 `creator` 添加到 `_registeredItemCreators` 映射中
        _registeredItemCreators[name] = std::move(creator);

        // 将类别（`category`）插入到 `_categories` 集合中
        // 这样可以确保类别在集合中唯一
        _categories.insert(category);

        // 将模型的名称和类别关联并存储在 `_registeredModelsCategory` 映射中
        // 这样可以方便以后通过名称找到模型的类别
        _registeredModelsCategory[name] = category;
    }
}

    // 曾经源码中用于 添加模块的 接口
    // 因为其不支持 模块的构造函数带有参数而被替换掉
    // template<typename ModelType>
    // void registerModel(QString const &category = "Nodes")
    // {
    //     RegistryItemCreator creator = []() { return std::make_unique<ModelType>(); };
    //     registerModel<ModelType>(std::move(creator), category);
    // }

    // template<typename ModelType, typename... Args>
    // void registerModel(QString const &category = "Nodes", Args&&... args)
    // {
    //     // 创建一个工厂函数，使用完美转发将所有参数传递给 ModelType 的构造函数
    //     RegistryItemCreator creator = [&, args = std::make_tuple(std::forward<Args>(args)...)]() mutable 
    //         {
    //             return std::apply([](auto&&... unpackedArgs) {
    //                 return std::make_unique<ModelType>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
    //             }, std::move(args));
    //         };
    //     // 使用修改后的 registerModel 函数注册 ModelType，传递 creator 和 category
    //     registerModel<ModelType>(std::move(creator), category);
    // }


    /**
     * @brief           注册节点流对象
     * @param category  节点所在分组
     * @param args      节点构造时的形参
     */
    template<typename ModelType, typename... Args>
    void registerModel(QString const &category = "Nodes", Args&&... args)
    {

        // 创建一个工厂函数，此处直接在 lambda 捕获列表中构造 std::tuple 来保存副本
        RegistryItemCreator creator = 
            // args = std::make_tuple(std::decay_t<Args>(args)...)函数接收到的所有参数 args... 打包进一个 tuple（元组）。
            // std::decay_t<Args>这是一个类型操作，它用来移除变量的引用性质和常量性质。这样做确保存入 tuple 的数据是"纯净的"，没有额外的属性，如引用或常量。
            [category, args = std::make_tuple(std::decay_t<Args>(args)...)]() mutable 
                {
                    // 使用 std::apply 和完美转发来构造 ModelType 的实例
                    return std::apply([](auto&&... unpackedArgs) {
                        // 在构造函数调用前显式复制每个参数，尤其是对于复杂对象如 QString
                        return std::make_unique<ModelType>(decltype(unpackedArgs)(unpackedArgs)...);
                    }, args
                    );
                };
        
        // 使用修改后的 registerModel 函数注册 ModelType，传递 creator 和 category
        registerModel<ModelType>(creator, category);
    }






template<typename ModelType, typename... Args>
void unRegisterModel(QString const &category = "Nodes", Args&&... args)
{
    // 创建一个工厂函数，使用完美转发将所有参数传递给 ModelType 的构造函数
    RegistryItemCreator creator = [&, args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
        return std::apply([](auto&&... unpackedArgs) {
            return std::make_unique<ModelType>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
        }, std::move(args));
    };

    // 使用修改后的 registerModel 函数注册 ModelType，传递 creator 和 category
    unRegisterModel<ModelType>(std::move(creator), category);
}
template<typename ModelType>
void unRegisterModel(RegistryItemCreator creator, QString const &category = "Nodes")
{
    QString const name = computeName<ModelType>(HasStaticMethodName<ModelType>{}, creator);
    // 确定已经注册过
    if (true == _registeredItemCreators.count(name)) {

        // // 如果没有注册过，则将 `creator` 添加到 `_registeredItemCreators` 映射中
        // _registeredItemCreators[name] = std::move(creator);
        _registeredItemCreators.erase(name);

        // 将类别（`category`）插入到 `_categories` 集合中
        // 这样可以确保类别在集合中唯一
        // _categories.insert(category);
        _categories.erase(category);  // 从_set_中删除指定的 category


        // 将模型的名称和类别关联并存储在 `_registeredModelsCategory` 映射中
        // 这样可以方便以后通过名称找到模型的类别
        // _registeredModelsCategory[name] = category;
        _registeredModelsCategory.erase(name);
    }
}


#if 0
  template<typename ModelType>
  void
  registerModel(RegistryItemCreator creator,
                QString const&      category = "Nodes")
  {
    registerModel<ModelType>(std::move(creator), category);
  }


  template <typename ModelCreator>
  void
  registerModel(ModelCreator&& creator, QString const& category = "Nodes")
  {
    using ModelType = compute_model_type_t<decltype(creator())>;
    registerModel<ModelType>(std::forward<ModelCreator>(creator), category);
  }


  template <typename ModelCreator>
  void
  registerModel(QString const& category, ModelCreator&& creator)
  {
    registerModel(std::forward<ModelCreator>(creator), category);
  }


  void
  registerTypeConverter(TypeConverterId const& id,
                        TypeConverter          typeConverter)
  {
    _registeredTypeConverters[id] = std::move(typeConverter);
  }

#endif

    std::unique_ptr<NodeDelegateModel> create(QString const &modelName);

    RegisteredModelCreatorsMap const &registeredModelCreators() const;

    RegisteredModelsCategoryMap const &registeredModelsCategoryAssociation() const;

    CategoriesSet const &categories() const;

#if 0
  TypeConverter
  getTypeConverter(NodeDataType const& d1,
                   NodeDataType const& d2) const;
#endif

private:
    RegisteredModelsCategoryMap _registeredModelsCategory;

    CategoriesSet _categories;

    RegisteredModelCreatorsMap _registeredItemCreators;

#if 0
  RegisteredTypeConvertersMap _registeredTypeConverters;
#endif

private:
    // If the registered ModelType class has the static member method
    // `static QString Name();`, use it. Otherwise use the non-static
    // method: `virtual QString name() const;`
    template<typename T, typename = void>
    struct HasStaticMethodName : std::false_type
    {};

    template<typename T>
    struct HasStaticMethodName<
        T,
        typename std::enable_if<std::is_same<decltype(T::Name()), QString>::value>::type>
        : std::true_type
    {};

    template<typename ModelType>
    static QString computeName(std::true_type, RegistryItemCreator const &)
    {
        return ModelType::Name();
    }

    template<typename ModelType>
    static QString computeName(std::false_type, RegistryItemCreator const &creator)
    {
        return creator()->name();
    }

    template<typename T>
    struct UnwrapUniquePtr
    {
        // Assert always fires, but the compiler doesn't know this:
        static_assert(!std::is_same<T, T>::value,
                      "The ModelCreator must return a std::unique_ptr<T>, where T "
                      "inherits from NodeDelegateModel");
    };

    template<typename T>
    struct UnwrapUniquePtr<std::unique_ptr<T>>
    {
        static_assert(std::is_base_of<NodeDelegateModel, T>::value,
                      "The ModelCreator must return a std::unique_ptr<T>, where T "
                      "inherits from NodeDelegateModel");
        using type = T;
    };

    template<typename CreatorResult>
    using compute_model_type_t = typename UnwrapUniquePtr<CreatorResult>::type;
};

} // namespace QtNodes