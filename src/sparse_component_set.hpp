#pragma once
#include <vector>
#include <typeinfo>
#include <stdexcept>
#include <cstdint>
#include <string>

namespace game_engine
{

	template <typename component_type>
	struct sparse_component_set
	{
	private:
		std::vector<uint32_t> m_sparse;
		std::vector<uint32_t> m_dense;
		std::vector<component_type> m_components;
		uint16_t m_size = 0;
		uint16_t m_capacity = 0;

	public:
		sparse_component_set() = default;
		sparse_component_set(uint16_t capacity) : m_capacity(capacity)
		{
			m_sparse.resize(capacity);
			m_dense.resize(capacity);
			m_components.resize(capacity);
		}
		sparse_component_set(const sparse_component_set &) = delete;
		sparse_component_set(sparse_component_set &&) = delete;
		sparse_component_set &operator=(const sparse_component_set &) = delete;
		sparse_component_set &operator=(sparse_component_set &&) = delete;
		~sparse_component_set() = default;

		/// @brief Reserve space for the specified number of entities
		/// @param capacity The number of entities to reserve space for
		void reserve(uint16_t capacity)
		{
			if (capacity <= m_capacity)
			{
				return;
			}
			m_capacity = capacity;
			m_sparse.resize(capacity, 0);
			m_dense.resize(capacity, 0);
			m_components.resize(capacity);
		}

		/// @brief Add an entity and its component to the set
		/// @param ent The entity to add
		/// @param comp The component to add

		void add(entity ent, component_type &comp)
		{
			if (contains(ent))
				return;
			if (ent >= m_capacity)
				reserve(ent + 1);
			m_sparse[ent] = m_size;
			m_dense[m_size] = ent;
			m_components[m_size] = std::move(comp);
			m_size++;
		}

		/// @brief Check if the set contains the specified entity
		/// @param ent The entity to check for
		void remove(entity ent)
		{
			if (!contains(ent))
				return;
			uint32_t last_entity = m_dense[(m_size - 1)];
			uint32_t index = m_sparse[ent];

			m_dense[index] = last_entity;
			m_sparse[last_entity] = index;

			m_size--;
		}

		/// @brief Update the component associated with the specified entity
		/// @param ent The entity to update the component for
		/// @param comp The component to update
		void update(entity ent, component_type &comp)
		{
			if (!contains(ent))
				return;
			m_components[m_sparse[ent]] = std::move(comp);
		}

		/// @brief Get the component asscociated with the specified entity.
		/// Will throw exception if the entity is not in the set
		/// @param ent The entity to get the component for
		/// @return The component associated with the entity
		component_type &get(entity ent)
		{
			if (!contains(ent))
			{
				char *error_string = (char *)malloc(100);
				sprintf(error_string, "sparse_component_set<%s> does not contain entity: %d\n", typeid(component_type).name(), ent);
				throw std::runtime_error(error_string);
			}
			return m_components[m_sparse[ent]];
		}

		/// @brief Get the number of entities in the set
		uint16_t size() const
		{
			return m_size;
		}

		/// @brief Get the current capacity of the set
		uint16_t capacity() const
		{
			return m_capacity;
		}

		/// @brief Check if the set contains the specified entity
		bool contains(entity ent) const
		{
			if (ent >= m_capacity)
				return false;
			return m_sparse[ent] < m_size; //&& m_dense[m_sparse[ent]] == ent;
		}

		/// @brief Get the entity at the specified index.
		/// Not likely to be used, as index in the dense array is not easily known. Access through entity ID is prefered.
		/// @param index The index of the entity to get from the dense array
		/// @return The entity at the specified index in the dense array
		uint32_t get_entity(uint32_t index) const
		{
			return m_dense[index];
		}

		/// @brief Get the component at the specified index
		/// Not likely to be used, as index in the dense array is not easily known. Access through entity ID is prefered.
		/// @param index The index of the component to get from the components array
		/// @return The component at the specified index in the components array
		component_type &get_component(uint32_t index)
		{
			return m_components[index];
		}

		/// @brief Get a pointer to the beginning of the dense array
		/// @return A pointer to the beginning of the dense array
		std::vector<uint32_t> *get_entities()
		{
			return &m_dense;
		}
	};
}