#include <Warp/Common.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_OWNING_POINTER_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_OWNING_POINTER_HPP

namespace Warp::Utilities
{
	template< typename StorageParameterType >
	struct OwningPointer
	{
		using StorageType = StorageParameterType;

		struct Reference {
			StorageType& resource;
		};

		OwningPointer() = delete;
		OwningPointer( const OwningPointer& ) = delete;
		OwningPointer( OwningPointer&& other ) : resource( other.resource ) {
			other.resource = nullptr;
		}
		OwningPointer( auto... initializers ) : resource( new StorageType{ initializers... } ) {}
		OwningPointer& operator=(const OwningPointer&) = delete;
		OwningPointer& operator=(OwningPointer&&) = delete;

		~OwningPointer() {
			delete resource;
		}

		protected: 
			StorageType* resource;

	};
} 
#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_OWNING_POINTER_HPP

