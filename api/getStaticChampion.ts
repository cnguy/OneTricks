import * as jsonfile from 'jsonfile'
import { LolStaticDataV3ChampionDto } from 'kayn/typings/dtos';

const staticChampions = jsonfile.readFileSync('./static_champions.json')

// Cached Static Champion keys for getStaticChampion.
const staticChampionKeys = Object.keys(staticChampions.data)

/**
 * getStaticChampion replaces the need for kayn.Static.Champion.get which gets
 * rate limited very easily.
 */
const getStaticChampion = (id: number): LolStaticDataV3ChampionDto => {
    const targetKey = staticChampionKeys.find(
        key => parseInt(staticChampions.data[key].id) === id,
    )!
    return staticChampions.data[targetKey]
    // It should always return.
}

export const getStaticChampionByName = (name: string): LolStaticDataV3ChampionDto => {
    const targetKey = staticChampionKeys.find(key => {
        if (
            staticChampions.data[key].key === 'MonkeyKing' &&
            name === 'Wukong'
        ) {
            return true
        }
        return (
            staticChampions.data[key].key.toLowerCase() === name.toLowerCase()
        )
    })
    return staticChampions.data[targetKey!]
}

export default getStaticChampion