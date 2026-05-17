<script setup lang="ts">
import { ref } from 'vue'

interface Reference {
  id: string
  author: string
  title: string
  year?: number
  source?: string
  url?: string
  doi?: string
  bibtex?: string
}

interface Props {
  references: Reference[]
  title?: string
  numbered?: boolean
}

const props = withDefaults(defineProps<Props>(), {
  title: 'References',
  numbered: true,
})

const copiedId = ref<string | null>(null)

function formatCitation(ref: Reference): string {
  const parts = [ref.author]
  if (ref.year) parts.push(`(${ref.year})`)
  parts.push(`*${ref.title}*`)
  if (ref.source) parts.push(ref.source)
  return parts.join('. ')
}

async function copyBibtex(ref: Reference) {
  if (!ref.bibtex) return
  try {
    await navigator.clipboard.writeText(ref.bibtex)
    copiedId.value = ref.id
    setTimeout(() => {
      if (copiedId.value === ref.id) copiedId.value = null
    }, 2000)
  } catch {
    // Silent fail on clipboard errors
  }
}
</script>

<template>
  <section class="wp-citations">
    <h3>{{ title }}</h3>
    <ol v-if="numbered" class="wp-citations-list">
      <li v-for="ref in references" :key="ref.id" class="wp-citation-item">
        <span class="citation-author">{{ ref.author }}</span>
        <span v-if="ref.year" class="citation-year"> ({{ ref.year }})</span>.
        <span class="citation-title">{{ ref.title }}</span>
        <span v-if="ref.source" class="citation-source">. {{ ref.source }}</span>
        <span v-if="ref.url || ref.doi" class="citation-link">
          <a :href="ref.url || `https://doi.org/${ref.doi}`" target="_blank" rel="noopener noreferrer">
            {{ ref.doi ? `DOI: ${ref.doi}` : 'Link' }}
          </a>
        </span>
        <button
          v-if="ref.bibtex"
          class="citation-copy-btn"
          @click="copyBibtex(ref)"
          :title="copiedId === ref.id ? 'Copied!' : 'Copy BibTeX'"
        >
          <svg v-if="copiedId === ref.id" width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
            <polyline points="20 6 9 17 4 12" />
          </svg>
          <svg v-else width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <rect x="9" y="9" width="13" height="13" rx="2" ry="2"/>
            <path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"/>
          </svg>
          {{ copiedId === ref.id ? 'Copied' : 'BibTeX' }}
        </button>
      </li>
    </ol>
    <ul v-else class="wp-citations-list">
      <li v-for="ref in references" :key="ref.id" class="wp-citation-item">
        <span class="citation-author">{{ ref.author }}</span>
        <span v-if="ref.year" class="citation-year"> ({{ ref.year }})</span>.
        <span class="citation-title">{{ ref.title }}</span>
        <span v-if="ref.source" class="citation-source">. {{ ref.source }}</span>
        <span v-if="ref.url || ref.doi" class="citation-link">
          <a :href="ref.url || `https://doi.org/${ref.doi}`" target="_blank" rel="noopener noreferrer">
            {{ ref.doi ? `DOI: ${ref.doi}` : 'Link' }}
          </a>
        </span>
        <button
          v-if="ref.bibtex"
          class="citation-copy-btn"
          @click="copyBibtex(ref)"
          :title="copiedId === ref.id ? 'Copied!' : 'Copy BibTeX'"
        >
          <svg v-if="copiedId === ref.id" width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
            <polyline points="20 6 9 17 4 12" />
          </svg>
          <svg v-else width="12" height="12" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <rect x="9" y="9" width="13" height="13" rx="2" ry="2"/>
            <path d="M5 15H4a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h9a2 2 0 0 1 2 2v1"/>
          </svg>
          {{ copiedId === ref.id ? 'Copied' : 'BibTeX' }}
        </button>
      </li>
    </ul>
  </section>
</template>
