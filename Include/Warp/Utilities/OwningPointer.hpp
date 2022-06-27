/*#include <Warp/Common.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_OWNING_POINTER_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_OWNING_POINTER_HPP

namespace Warp::Utilities
{
	// Intended to share mutable state BUT NOT MEMORY OWNERSHIP. //
	template< typename StorageParameterType >
	struct OwningPointer
	{
		using StorageType = StorageParameterType;
		using ThisType = OwningPointer< StorageType >;

		struct Reference
		{

			~Reference()
			{
				if( owner != nullptr )
				{
				}
			protected: 
				ThisType* owner;
				StorageType& resource;
		};

		OwningPointer() = delete;
		OwningPointer( const OwningPointer& ) = delete;
		OwningPointer( OwningPointer&& other ) 
				: resource( other.resource ), 
				owner( &this ) {
			other.update( &this );
		}
		OwningPointer( auto... initializers ) 
				: resource( new StorageType{ initializers... } ), 
				owner( &this ) 
		{}
		OwningPointer& operator=(const OwningPointer&) = delete;
		OwningPointer& operator=(OwningPointer&&) = delete;

		~OwningPointer() {
			delete resource;
		}

		StorageType& get_resource() {
			return *resource;
		}

		const StorageType& get_resource() const {
			return *resource;
		}



		protected: 
			StorageType* resource;
			ThisType** owner;
			std::vector< Reference& > references;

			void update( ThisType** owner )
			{
				this->owner == owner;
				resource = ( this == this->owner ) ? resource : nullptr;
				for( auto reference : references ) {
					reference.owner = *this->owner;
					this->owner->references.push_back( reference );
				}
			}
	};
} 
#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_OWNING_POINTER_HPP
*/
