#include <Server_Common/Common.h>
#include <Server_Common/CommonNetwork.h>
#include <Server_Common/Database.h>
#include <Server_Common/GamePacketNew.h>
#include <Server_Common/Logger.h>
#include <Server_Common/ExdData.h>
#include <Server_Common/PacketContainer.h>

#include <boost/format.hpp>

#include "src/servers/Server_Zone/Network/GameConnection.h"

#include "src/servers/Server_Zone/Session.h"
#include "src/servers/Server_Zone/Zone/Zone.h"
#include "src/servers/Server_Zone/Zone/ZonePosition.h"
#include "src/servers/Server_Zone/ServerZone.h"
#include "src/servers/Server_Zone/Zone/ZoneMgr.h"

#include "src/servers/Server_Zone/Network/PacketWrappers/ServerNoticePacket.h"
#include "src/servers/Server_Zone/Network/PacketWrappers/ActorControlPacket142.h"
#include "src/servers/Server_Zone/Network/PacketWrappers/ActorControlPacket143.h"
#include "src/servers/Server_Zone/Network/PacketWrappers/ActorControlPacket144.h"

#include "src/servers/Server_Zone/DebugCommand/DebugCommandHandler.h"
#include "src/servers/Server_Zone/Actor/Player.h"
#include "src/servers/Server_Zone/Inventory/Inventory.h"
#include "src/servers/Server_Zone/Forwards.h"

extern Core::Logger g_log;
extern Core::Db::Database g_database;
extern Core::ServerZone g_serverZone;
extern Core::ZoneMgr g_zoneMgr;
extern Core::Data::ExdData g_exdData;
extern Core::DebugCommandHandler g_gameCommandMgr;

using namespace Core::Common;
using namespace Core::Network::Packets;
using namespace Core::Network::Packets::Server;


void Core::Network::GameConnection::inventoryModifyHandler( const Packets::GamePacket& inPacket,
                                                            Entity::PlayerPtr pPlayer )
{
    uint32_t seq = inPacket.getValAt< uint32_t >( 0x20 );
    uint8_t action = inPacket.getValAt< uint8_t >( 0x24 );
    uint8_t fromSlot = inPacket.getValAt< uint8_t >( 0x30 );
    uint8_t toSlot = inPacket.getValAt< uint8_t >( 0x44 );
    uint16_t fromContainer = inPacket.getValAt< uint16_t >( 0x2C );
    uint16_t toContainer = inPacket.getValAt< uint16_t >( 0x40 );

    GamePacketNew< FFXIVIpcInventoryActionAck > ackPacket( pPlayer->getId() );
    ackPacket.data().sequence = seq;
    ackPacket.data().type = 7;
    pPlayer->queuePacket( ackPacket );


    g_log.debug( inPacket.toString() );
    g_log.debug( "InventoryAction: " + std::to_string( action ) );

    // TODO: other inventory operations need to be implemented
    switch( action )
    {

        case 0x07: // discard item action
        {
            pPlayer->getInvetory()->discardItem( fromContainer, fromSlot );
        }
            break;

        case 0x08: // move item action
        {
            pPlayer->getInvetory()->moveItem( fromContainer, fromSlot, toContainer, toSlot );
        }
            break;

        case 0x09: // swap item action
        {
            pPlayer->getInvetory()->swapItem( fromContainer, fromSlot, toContainer, toSlot );
        }
            break;

        case 0x0C: // merge stack action
        {

        }
            break;

        case 0x0A: // split stack action
        {

        }
            break;

        default:

            break;
    }

    pPlayer->setSyncFlag( PlayerSyncFlags::Status );

}
